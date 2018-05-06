/* Lua socket module */

#include <sys/lua.h>
#include <sys/module.h>
#include <sys/socketvar.h>
#include <netinet/in.h>

#include <lua.h>
#include <lauxlib.h>

/* our kernel 'stdio' :) */
#include <stdio.h>

MODULE(MODULE_CLASS_MISC, luasocket, "lua");

#define BUF_SIZE 1024

#define LUA_SOCK "luasock"

struct luasocket {
	struct socket *so;
	int fd;
	int dom;
	int typ;
};

static struct luasocket*
new_lsocket(lua_State *L)
{
	struct luasocket *lsock;

	lsock = (struct luasocket*) lua_newuserdata(L, sizeof(struct luasocket));
	luaL_setmetatable(L, LUA_SOCK);

	return lsock;
}

static int
socket_new(lua_State *L, int domain, int type)
{
	struct luasocket *lsock;

	lsock = new_lsocket(L);
	fsocreate(domain, &lsock->so, type, 0, &lsock->fd);
	lsock->dom = domain;

	return 1;
}

static int
socket_udp(lua_State *L)
{
	return socket_new(L, AF_INET, SOCK_DGRAM);
}

static int
socket_tcp(lua_State *L)
{
	return socket_new(L, AF_INET, SOCK_STREAM);
}

static int
socket_local(lua_State *L)
{
	return socket_new(L, AF_LOCAL, SOCK_STREAM);
}

static int
socket_raw(lua_State *L)
{
	return socket_new(L, AF_INET, SOCK_RAW);
}

inline static struct luasocket*
checksock(lua_State *L, int idx)
{
	struct luasocket *l;

	if ((l = (struct luasocket*) lua_touserdata(L, idx)) == NULL)
		luaL_error(L, "invalid Lua socket");

	return l;
}

static int
socket_close(lua_State *L)
{
	struct luasocket *luasocket;
	int err;

	luasocket = checksock(L, 1);
	if ((err = close(luasocket->fd)) != 0)
		luaL_error(L, "could not close socket (error %d)", err);
	lua_pushboolean(L, 1);

	return 1;
}

typedef int BindOrConnect(struct socket*, struct sockaddr*, struct lwp*);

static int
bindOrConn_inet(BindOrConnect bindOrConn, struct socket* so, const char *addr,
                  int port)
{
	struct sockaddr_in in4addr;
	int err;

	memset(&in4addr, 0, sizeof(in4addr));
	in4addr.sin_len = sizeof(struct sockaddr_in);
	in4addr.sin_addr.s_addr = inet_addr(addr);
	in4addr.sin_port = htons(port);
	in4addr.sin_family = AF_INET;

	/* XXX dealing with inconsistent locking protocol */
	if (bindOrConn == soconnect)
		solock(so);
	err = bindOrConn(so, (struct sockaddr*) &in4addr, curlwp);
	if (bindOrConn == soconnect)
		sounlock(so);

	return err;
}

static int
socket_bindOrConn(BindOrConnect bindOrConn, lua_State *L)
{
	struct luasocket *luasocket;
	const char *addr;
	int err;

	luasocket = checksock(L, 1);
	addr = luaL_checkstring(L, 2);

	switch(luasocket->dom) {
		case AF_INET: {
			int port = luaL_checkinteger(L, -1);
			err = bindOrConn_inet(bindOrConn, luasocket->so, addr, port);
			break;
		}
	}

	if (err == 0)
		lua_pushvalue(L, 1);
	else
		lua_pushnil(L);

	return 1;
}

inline static int
socket_connect(lua_State *L)
{
	return socket_bindOrConn(soconnect, L);
}

inline static int
socket_bind(lua_State *L)
{
	return socket_bindOrConn(sobind, L);
}

static int
socket_listen(lua_State *L)
{
	struct luasocket *l;
	int err, bl;

	l = checksock(L, 1);
	bl = luaL_checkinteger(L, 2);

	if(solisten(l->so, bl, curlwp) == 0)
		lua_pushvalue(L, 1);
	else
		lua_pushnil(L);

	return 1;
}

static int
socket_accept(lua_State *L)
{
	struct luasocket *l, *cli;
	struct sockaddr_in sockaddr;
	int csfd;

	l = checksock(L, 1);
	cli = new_lsocket(L);

	solock(l->so);
	csfd = soaccept(l->so, (struct sockaddr*) &sockaddr);
	sounlock(l->so);

	cli->fd = csfd;

	return 1;
}

static int
socket_shutdown(lua_State *L)
{
	struct luasocket *l;
	const char *howstr;
	int how;

	l = checksock(L, 1);
	howstr = luaL_checkstring(L, 2);

	/* disallow further receives */
	if (strcmp(howstr, "recv") == 0)
		how = 0;
	/* disallow further sends */
	else if (strcmp(howstr, "send") == 0)
		how = 1;
	/* disallow further sends & receives */
	else
		how = 2;

	solock(l->so);
	lua_pushboolean(L, soshutdown(l->so, how));
	sounlock(l->so);

	return 1;
}

static int
socket_write(lua_State *L)
{
	struct luasocket* luasocket;
	const char *buff;
	int cnt, len;

	luasocket = checksock(L, 1);
	buff = luaL_checkstring(L, 2);

	if(lua_gettop(L) == 3)
		len = luaL_checkinteger(L, 3);
	else
		len = strlen(buff);

	cnt = write(luasocket->fd, buff, len);
	lua_pushinteger(L, cnt);

	return 1;
}

static int
socket_read(lua_State *L)
{
	struct luasocket* luasocket;
	luaL_Buffer buff;
	int cnt;

	luasocket = checksock(L, 1);
	cnt = read(luasocket->fd, luaL_buffinitsize(L, &buff, BUF_SIZE), BUF_SIZE);

	lua_pushinteger(L, cnt);
	luaL_pushresultsize(&buff, cnt);

	return 1;
}

static int
luaopen_socket(lua_State *L)
{
	const luaL_Reg socket_lib[] = {
		{"udp", socket_udp},
		{"tcp", socket_tcp},
		{"raw", socket_raw},
		{"local", socket_local},
		{"close", socket_close},
		{NULL, NULL}
	};

	const luaL_Reg socket_meta[] = {
		{"bind", socket_bind},
		{"connect", socket_connect},
		{"listen", socket_listen},
		{"accept", socket_accept},
		{"shutdown", socket_shutdown},
		{"close", socket_close},
		{"write", socket_write},
		{"read", socket_read},
		{"__gc", socket_close},
		{NULL, NULL}
	};

	luaL_newlib(L, socket_lib);
	luaL_newmetatable(L, LUA_SOCK);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, socket_meta, 0);
	lua_pop(L, 1);

	return 1;
}

static int
luasocket_modcmd(modcmd_t cmd, void *opaque)
{
	int error;
	switch (cmd) {
	case MODULE_CMD_INIT:
		error = klua_mod_register("socket", luaopen_socket);
		break;
	case MODULE_CMD_FINI:
		error = klua_mod_unregister("socket");
		break;
	default:
		error = ENOTTY;
	}
	return error;
}

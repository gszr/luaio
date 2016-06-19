/* Lua socket module */

#include <sys/lua.h>
#include <sys/module.h>
#include <sys/socketvar.h>
#include <netinet/in.h>

#include <lua.h>
#include <lauxlib.h>

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

inline static struct luasocket*
checksock(lua_State *L, int idx)
{
	struct luasocket *l;

	if ((l = (struct luasocket*) lua_touserdata(L, idx)) == NULL)
		luaL_error(L, "invalid Lua socket");

	return l;
}

static int
socket_new(lua_State *L)
{
	struct luasocket *luasocket;
	struct socket *so;
	const char *domain;
	const char *type;
	int sfd;
	int err;
	int dom = 0;
	int typ = 0;

	domain = luaL_checkstring(L, 1);
	type   = luaL_checkstring(L, 2);

	if (strcmp(domain, "inet4") == 0)
		dom = AF_INET;
	else if (strcmp(domain, "inet6"))
		dom = AF_INET6;
	else if (strcmp(domain, "local"))
		dom = AF_LOCAL;
	else
		luaL_error(L, "'%s' domain not supported", domain);

	if (strcmp(type, "dgram") == 0 || strcmp(type, "datagram") == 0)
		typ = SOCK_DGRAM;
	else if (strcmp(type, "stream"))
		typ = SOCK_STREAM;
	else
		luaL_error(L, "'%s' type not supported", type);

	if ((err = fsocreate(dom, &so, typ, 0, &sfd)))
		luaL_error(L, "could not create socket (error %d)", err);

	luasocket = (struct luasocket*)
		lua_newuserdata(L, sizeof(struct luasocket));
	luaL_setmetatable(L, LUA_SOCK);

	luasocket->so  = so;
	luasocket->fd  = sfd;
	luasocket->dom = dom;
	luasocket->typ = typ;

	return 1;
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

	lua_pushboolean(L, err);
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
	lua_pushboolean(L, solisten(l->so, bl, curlwp) == 0);

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
		{"new", socket_new},
		{"close", socket_close},
		{NULL, NULL}
	};

	const luaL_Reg socket_meta[] = {
		{"bind", socket_bind},
		{"connect", socket_connect},
		{"listen", socket_listen},
		{"close", socket_close},
		{"write", socket_write},
		{"read", socket_read},
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

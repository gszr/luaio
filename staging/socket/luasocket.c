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

	if ((luasocket = (struct luasocket *) lua_touserdata(L, 1)) == NULL)
		luaL_error(L, "invalid Lua socket");
	if ((err = close(luasocket->fd)) != 0)
		luaL_error(L, "could not close socket (error %d)", err);
	lua_pushboolean(L, 1);

	return 1;
}

typedef int BindOrConnect(struct socket*, struct sockaddr*, struct lwp*);

static int
socket_bind_or_conn(BindOrConnect handler, lua_State *L)
{
	struct luasocket *luasocket;
	const char *addr;
	int err;

	if ((luasocket = (struct luasocket *) lua_touserdata(L, 1)) == NULL)
		luaL_error(L, "invalid socket");
	addr = luaL_checkstring(L, 2);

	// XXX inet4 and inet6 could share code
	if (luasocket->dom == AF_INET) {
		struct sockaddr_in in4addr;

		if (lua_gettop(L) != 3)
			luaL_error(L, "inet4 expects an IP address and a port number");

		memset(&in4addr, 0, sizeof(in4addr));
		in4addr.sin_len = sizeof(struct sockaddr_in);
		in4addr.sin_addr.s_addr = inet_addr(addr);
		in4addr.sin_port = htons(luaL_checkinteger(L, -1));
		in4addr.sin_family = AF_INET;

		solock(luasocket->so);
		if ((err = handler(luasocket->so, (struct sockaddr*) &in4addr,
			curlwp)) != 0)
			luaL_error(L, "could not connect");
		sounlock(luasocket->so);

		lua_pushboolean(L, 1);
	}

	return 1;
}

inline static int
socket_connect(lua_State *L)
{
	return socket_bind_or_conn(soconnect, L);
}

inline static int
socket_bind(lua_State *L)
{
	return socket_bind_or_conn(sobind, L);
}

static int
socket_write(lua_State *L)
{
	struct luasocket* luasocket;
	const char *buff;
	int cnt;

	if ((luasocket = (struct luasocket*) lua_touserdata(L, 1)) == NULL)
		luaL_error(L, "invalid socket");
	buff = luaL_checkstring(L, 2);

	//XXX handle binary data
	cnt = write(luasocket->fd, buff, strlen(buff));
	lua_pushinteger(L, cnt);

	return 1;
}

static int
socket_read(lua_State *L)
{
	struct luasocket* luasocket;
	//XXX probably not a good idea; lua_pushlstring?
	char buff[BUF_SIZE];
	int cnt;

	if ((luasocket = (struct luasocket*) lua_touserdata(L, 1)) == NULL)
		luaL_error(L, "invalid socket");

	cnt = read(luasocket->fd, &buff, BUF_SIZE);

	lua_pushinteger(L, cnt);
	lua_pushlstring(L, buff, cnt);

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

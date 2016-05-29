/* Lua sockets module */

#include <sys/lua.h>
#include <sys/module.h>
#include <sys/socketvar.h>

#include <lua.h>
#include <lauxlib.h>
#include <mappings.h>

MODULE(MODULE_CLASS_MISC, luasockets, "lua");

static int
sockets_socket(lua_State *L)
{
	struct socket *so;
	
	const char *domain;
	const char *type;

	int sfd;
	int err;
	int dom = 0;
	int typ = 0;

	domain = luaL_checkstring(L, 1);
	type   = luaL_checkstring(L, 2);

	if (strcmp(domain, "inet") == 0 || strcmp(domain, "inet4"))
		dom = AF_INET;
	else if (strcmp(domain, "inet6"))
		dom = AF_INET6;
	else if (strcmp(domain, "local"))
		dom = AF_LOCAL;
	else
		luaL_error(L, "'%s' domain not supported\n", domain);

	if (strcmp(type, "dgram") == 0 || strcmp(type, "datagram") == 0)
		typ = SOCK_DGRAM;
	else if (strcmp(type, "stream"))
		typ = SOCK_STREAM;
	else
		luaL_error(L, "'%s' type not supported\n", type);

	if ((err = fsocreate(dom, &so, typ, 0, &sfd)))
		luaL_error(L, "could not create socket (error %d)", err);

	lua_pushlightuserdata(L, so);
	return 1;
}

static int
sockets_close(lua_State *L)
{
	int err;
	struct socket *so;
	if ((so = (struct socket *) lua_touserdata(L, 1)))
		luaL_error(L, "invalid socket\n");
	if ((err = soclose(so)))
		luaL_error(L, "could inot close socket (error %d)\n", err);
	lua_pushboolean(L, 1);
	return 1;
}

static int
luaopen_sockets(lua_State *L)
{
	const luaL_Reg sockets_lib[] = {
		{"socket", sockets_socket},
		{"close", sockets_close},
		{ NULL, NULL }
	};

	luaL_newlib(L, sockets_lib);

	return 1;
}

static int
luasockets_modcmd(modcmd_t cmd, void *opaque)
{
	int error;
	switch (cmd) {
	case MODULE_CMD_INIT:
		error = klua_mod_register("sockets", luaopen_sockets);
		break;
	case MODULE_CMD_FINI:
		error = klua_mod_unregister("sockets");
		break;
	default:
		error = ENOTTY;
	}
	return error;
}

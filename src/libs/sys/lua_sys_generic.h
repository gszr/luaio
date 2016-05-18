
#ifndef _LUA_SYS_GENERIC_
#define _LUA_SYS_GENERIC_

int lua_dofileread(int, struct file *, void *, size_t, off_t *, int,
                   register_t *);
int lua_dofilewrite(int, struct file *, const void *, size_t, off_t *, int,
                    register_t *);

#endif

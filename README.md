## Kernel Lua I/O Bindings

The goal of this project is to build I/O - currently file systems and sockets -
bindings for NetBSD kernel Lua.

### Research

Grasping kernel subsystems is a daunting task; this section contains a few
summaries, byproduct of code reading and research into the I/O and networking
subsystems of the NetBSD kernel.

* [VFS and NetBSD VFS implementation overview](https://github.com/salazar/luaio/blob/master/docs/r1_vfs.txt)
* [NetBSD I/O Subsystem  overview](https://github.com/salazar/luaio/blob/master/docs/r2_io.txt)
* NetBSD Networking Subsystem overview (soon)
* [Kernel Lua I/O API Specification](https://github.com/salazar/luaio/blob/master/docs/r4_api.txt)
* [Socket module API](https://github.com/salazar/luaio/blob/master/docs/r6_final_socket_api.txt)

### Implementation

Directories under [src](src):
* io: port of Lua 'io' module to NetBSD kernel Lua
* socket: socket library to kernel Lua
* staging: contains any code I wrote to see how things work
* progs: some Lua programs using io and socket libraries


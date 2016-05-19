# luaio - kernel Lua I/O API

## Welcome to my Google Summer of Code LabLua '16 project!

I'm building NetBSD kernel I/O (files and sockets) bindings for kernel Lua 
under [lneto](https://github.com/lneto)'s mentorship.

'docs' houses any documentation produced during the project; at the moment,
I wrote the following reports:

  * Report 1: [VFS and NetBSD VFS implementation 
  overview](https://github.com/salazar/luaio/blob/master/docs/r1_vfs.txt)
  * Report 2: [NetBSD I/O Subsystem 
  overview](https://github.com/salazar/luaio/blob/master/docs/r2_io.txt)
  * Report 3: NetBSD Networking Subsystem overview (soon)
  * Report 4: [Kernel Lua I/O API 
  Specification](https://github.com/salazar/luaio/blob/master/docs/r4_api.txt)

'src' contains source code

  * libs: general I/O code used to implement the Lua bindings
    - root: io.{c,h} is the general I/O code; e.g., contains 'write' and
      'read' calls to deal with regular files and read/write
    - sys: bits of code I had to adapt from the NetBSD tree
    - sockets: code specific to socket kernel object represented by a
      file descriptor
  * bindings: kernel Lua bindings; they are loadable kernel modules
  * staging: contains any code I wrote to see how things work; not (yet)
    meant to be in 'src'

https://goo.gl/g8Mb2M

# Lua standard libraries port

This is a port of Lua standard libraries, including 'io', to kernel Lua; it
adds support to

  * io: open, close, input, output, lines, read, write, tmpfile, type
  * os: clock, time, difftime, remove, tmpname
  * math: fmod, max, min, random, randomseed, type, huge, maxinteger, mininteger

The port is contained in [libs_port.patch](libs_port.patch), which should be
applied against the NetBSD source tree. (The 'include' will be created when the
patch is applied; it is here for ease of reference.)

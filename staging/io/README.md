# Generic I/O layer

This is a draft implemention of a binding to NetBSD I/O base functionality;
kopen works for regular files; kclose should work at least for regular files,
but I expect it to work for sockets and other kernel objects -- same goes to
kread and kwrite.

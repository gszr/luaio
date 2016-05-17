#include <sys/filedesc.h>
#include <sys/fcntl.h>

#include "io.h"

int
kopen(const char *path, int flags, ...)
{
	int fd;
	mode_t perms = 666;
	va_list va;

	if (flags & O_CREAT) {
		va_start(va, flags);
		perms = va_arg(va, mode_t);
		va_end(va);
	}

	if (fd_open(path, flags, perms, &fd))
		return -1;
	return fd;
}

int
kclose(int fd)
{
	if (fd_getfile(fd) == NULL)
		return EBADF;
	return fd_close(fd);
}

ssize_t
kread(int fd, void *buf, size_t count)
{
	return 0;
}

ssize_t
kwrite(int fd, const void *buf, size_t count)
{
	return 0;
}

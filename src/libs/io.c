#include <sys/filedesc.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "io.h"
#include "lua_sys_generic.h"

int
kopen(const char *path, int flags, ...)
{
	int fd;
	mode_t perms;
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
	ssize_t ret;
	file_t *fp;

	if ((fp = fd_getfile(fd)) == NULL)
		return (EBADF);

	if (lua_dofileread(fd, fp, buf, count, &fp->f_offset, FOF_UPDATE_OFFSET,
	    &ret))
		return -1;
	return ret;
}

ssize_t
kwrite(int fd, const void *buf, size_t count)
{
	ssize_t ret;
	file_t *fp;

	if ((fp = fd_getfile(fd)) == NULL)
	return (EBADF);

	if (lua_dofilewrite(fd, fp, buf, count, &fp->f_offset, FOF_UPDATE_OFFSET,
	    &ret))
		return -1;
	return ret;
}

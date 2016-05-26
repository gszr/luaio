#include <sys/filedesc.h>
#include <sys/file.h>
#include <sys/syscallargs.h>
#include <sys/vfs_syscalls.h>

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

int
kseek(int fd, off_t offset, int whence)
{
	struct sys_lseek_args sargs;
	register_t ret;

	memset(&sargs, 0, sizeof sargs);
	SCARG(&sargs, fd) = fd;
	SCARG(&sargs, PAD) = 0;
	SCARG(&sargs, offset) = offset;
	SCARG(&sargs, whence) = whence;

	return sys_lseek(curlwp, &sargs, &ret);
}

int
kunlink(const char *path)
{
	return do_sys_unlink(path, UIO_SYSSPACE);
}

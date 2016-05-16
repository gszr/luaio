#include <sys/vfs_syscalls.h>
#include <sys/syscallargs.h>
#include <sys/lwp.h>

#include "io.h"

int
kopen(const char *path, int flags)
{
	int fd;
	struct sys_open_args oa;

	SCARG(&oa, path)  = path;
	SCARG(&oa, flags) = flags;
	SCARG(&oa, mode)  = 0700;

	if (sys_open(curlwp, &oa, &fd))
		return -1;
	return fd;
}

int
kclose(int fd)
{
	struct sys_close_args ca;
	SCARG(&ca, fd) = fd;
	return sys_close(curlwp, &ca, NULL);
}

ssize_t
kread(int, void *buf, size_t count)
{
	return 0;
}

ssize_t
kwrite(int, const void *buf, size_t count)
{
	return 0;
}


#ifndef _IO_H_
#define _IO_H_

#include <sys/stat.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/syscallargs.h>
#include <sys/vfs_syscalls.h>

#include "sys/sys_generic.h"
#include "tmpnam.h"

#define EOF	-1
#define BUFSIZ	1024
#define FILE long

#define stdin	NULL
#define stdout	NULL
#define stderr	NULL

#define ferror(fd)	0
#define fflush(fd)	0
#define clearerr(fd)

static int mode2flag(const char *mode) {
	int flag;
	if (*mode == 'r')
		flag = O_RDONLY;
	else if (*mode == 'w')
		flag = O_WRONLY;
	else if (*mode == 'a')
		flag |= O_WRONLY | O_APPEND;
	if (*(mode + 1) != '\0' && *(mode + 1) == '+') {
		flag = O_RDWR;
		if (*mode == 'a')
			flag |= O_APPEND;
		else if (*mode == 'w')
			flag |= O_CREAT;
	}
	return flag;
}

inline static long open(const char *path, int flags) {
	int fd;

	if (fd_open(path, flags, ACCESSPERMS, &fd))
		return -1;
	return fd;

}
#define fopen(path, mode) (long*)open(path, mode2flag(mode))

inline static int close(int fd) {
	if (fd_getfile(fd) == NULL)
		return EBADF;
	return fd_close(fd);
}
#define fclose(fp)	close((long) fp)

inline static ssize_t read(int fd, void *buf, size_t count) {
	ssize_t ret;
	file_t *fp;

	if ((fp = fd_getfile(fd)) == NULL)
		return (EBADF);

	if (_dofileread(fd, fp, buf, count, &fp->f_offset, FOF_UPDATE_OFFSET,
	    &ret))
		return -1;
	return ret;

}
#define fread(buff, sz, nb, fp)	read((long) fp, buff, sz * nb)

inline static ssize_t write(int fd, const void *buf, size_t count) {
	ssize_t ret;
	file_t *fp;

	if ((fp = fd_getfile(fd)) == NULL)
	return (EBADF);

	if (_dofilewrite(fd, fp, buf, count, &fp->f_offset, FOF_UPDATE_OFFSET,
	    &ret))
		return -1;
	return ret;

}
#define fwrite(buff, sz, nb, fp)	write((long) fp, buff, sz * nb)

inline static int _fseek(long fd, off_t offset, int whence) {
	struct sys_lseek_args sargs;
	register_t ret = -1;

	memset(&sargs, 0, sizeof sargs);
	SCARG(&sargs, fd) = fd;
	SCARG(&sargs, PAD) = 0;
	SCARG(&sargs, offset) = offset;
	SCARG(&sargs, whence) = whence;

	sys_lseek(curlwp, &sargs, &ret);
	return ret;
}
#define fseek(fp, offset, whence) _fseek((long) fp, offset, whence)
#define ftell(fp) fseek((long)fp, 0, SEEK_CUR)

inline static int _getc(int fd) {
	int c;
	if (read(fd, &c, 1) != 1)
		c = -1;
	return c;
}

#define getc(fp)	_getc((long)fp)

#define ungetc(ch, fd) fseek((long)fd, -1, SEEK_CUR)

inline static int _eof(int fd) {
	int c = getc(fd) == -1;
	if (c) fseek(fd, -1, SEEK_CUR);
	return c;
}
#define feof(fp) _eof((long)fp)

inline static int remove(const char *path) {
	return do_sys_unlink(path, UIO_SYSSPACE);
}

#define fprintf(fp, fmt, val) \
		({ \
			char s[strlen(fmt)] = {'\0'}; \
			int l = snprintf(s, sizeof s, fmt, val); \
			fwrite(s, sizeof s, 1, (long) fp); \
		})

#define L_tmpnam    32

#define tmpfile() \
		({ \
			char b[L_tmpnam] = {'\0'}; \
			fopen(tmpnam(b), "w+"); \
		})

#define tmpnam(nam) \
		({ \
			strcpy(nam, "/tmp/klua_XXXXXX"); \
			ktmpnam(nam); \
		})

#endif

/*
 * Copyright (c) 2015-2016 Guilherme Salazar
 * Copyright (c) 1983-2015 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*
 * Obtained and adapted from
 * http://www.retro11.de/ouxr/43bsd/usr/src/lib/libc/gen/mkstemp.c.html
 */

#ifndef _KTMPNAM_H
#define _KTMPNAM_H

#include <sys/filedesc.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

static inline char* ktmpnam(char *tmp) {
	long  rnd;
	char  ch, *s;
	int   fd;

	s   = tmp;
	rnd = random();

	while (*s++)
		;

	s--;
	while (*--s == 'X') {
		*s = (rnd % 10) + '0';
		rnd /= 10;
	}
	s++;

	ch = 'a';
	while ((fd_open(tmp, O_CREAT|O_EXCL|O_RDWR, ALLPERMS, &fd)) != 0) {
		if (ch == 'z')
			return NULL;
		*s = ch++;
	}
	if (fd_getfile(fd) != NULL)
		fd_close(fd);

	return tmp;
}

#endif

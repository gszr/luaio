#ifndef _IO_H_
#define _IO_H_

int kopen(const char *, int, ...);
int kclose(int);

ssize_t kread(int, void *, size_t);
ssize_t kwrite(int, const void *, size_t);

int kseek(int, off_t, int);

int kunlink(const char*);

#define kungetc(fd) (kseek(fd, -1, SEEK_CUR))
/* XXX ({...}) is a gcc extension */
#define kgetc(fd) ({int c; if(kread(fd, &c, 1) != 1) c = -1; c;})

#endif

#ifndef _IO_H_
#define _IO_H_

int kopen(const char *, int, ...);
int kclose(int);

ssize_t kread(int, void *, size_t);
ssize_t kwrite(int, const void *, size_t);

#endif

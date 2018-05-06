#ifndef _ERRNO_H_
#define _ERRNO_H_

/* XXX find a better way to handle errors ;P */
#define errno  -1
#define strerror(errno) "kernel lua: error"

#endif

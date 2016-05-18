
#ifndef _SOCKETS_H
#define _SOCKETS_H

// main API functions
int socket(int, int, int);
int bind(int, const struct sockaddr*, socklen_t);
int listen(int, int);
int connect(int, const struct sockaddr*, socklen_t addrlen);
int accept(int, struct sockaddr*, socklen_t);
int shutdown(int, int);

// build on top of read/write
ssize_t recv(int, void*, size_t, int);
ssize_t recvfrom(int, void*, size_t, int, struct sockaddr*, socklen_t);
ssize_t recvmsg(int, struct msghdr *, int);

ssize_t send(int, const void*, size_t, int);
ssize_t sendto(int, const void*, size_t, int, const struct sockaddr*, socklen_t);
ssize_t sendmsg(int, const struct msghdr*, int);

#endif

-- read stuff from TCP socket

print(require'socket'.tcp():bind("192.168.33.11", 8080):accept():read())

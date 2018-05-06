-- write stuff to UDP socket

print(require'socket'.udp():connect("192.168.33.1", 8080):write("alo"))

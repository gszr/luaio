local socket = require'socket'

local s = socket.new("inet4", "dgram")

s:bind("192.168.33.11", 8080)
print("read: " .. s:read())

s:close()

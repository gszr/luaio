local socket = require'socket'

local s = socket.new("inet4", "dgram")

s:connect("192.168.33.1", 8080)
print("written: " .. s:write("alo"))

s:close()

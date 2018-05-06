-- copy one file to another
-- all at once

local finp = io.open("big.txt", 'r')
local fout = io.open("out.txt", 'w+')

fout:write(finp:read('a')) 

finp:close()
fout:close()

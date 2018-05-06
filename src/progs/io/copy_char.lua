-- copy one file to another
-- char by char

local finp = io.open("big.txt", 'r')
local fout = io.open("out.txt", 'w+')

repeat
	local char = finp:read(1)
	if char then fout:write(char) end
until char == nil

finp:close()
fout:close()

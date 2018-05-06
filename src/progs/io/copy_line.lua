-- copy one file to another
-- line by line

local finp = "big.txt"
local fout = io.open("out.txt", 'w+')

for line in io.lines(finp) do
	fout:write(line) 
end

fout:close()

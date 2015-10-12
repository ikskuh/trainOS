io.write("Input stacktrace from kernel:\n")
local line = io.read("*line")

for addr in line:gmatch("(%w+)") do
--	io.write(("-"):rep(80).."\n")
--	io.write("address: " .. addr .. "\n")
--	io.write(("-"):rep(80).."\n")
--	os.execute("objdump -d kernel | grep "..addr:lower().." -A 3 -B 10")
	os.execute([[objdump -d kernel | grep -i ]]..addr..[[ -B 50 | grep -e ">:$" | tail -n 1 | sed -E "s|.*<(.*)>:|\1|" | c++filt]])
end

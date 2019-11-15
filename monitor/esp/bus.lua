
dofile('net.lua')

local function split(s, sep)
    local fields = {}
    local sep = sep or " "
    local pattern = string.format("([^%s]+)", sep)
    string.gsub(s, pattern, function(c) fields[#fields + 1] = c end)
    return fields
end

local function do_send(str)
    local parts = split(str)
    if parts[1]=='metric' then
        send_level(parts[2], parts[3])
    else
        print('Unknown send type '..parts[1])
    end
end

local function do_cmd(str)
    print('Command '..str..' (not implemented)')
end

local function process_data(data)    
    if data:byte(1)==62 then -- >
        if data:byte(2)==115 then -- s
            return do_send(data:sub(3))
        elseif data:byte(2)==99 then -- c
            return do_cmd(data:sub(3))
        else
            print('Wrong command '..data:sub(2))
        end
    else
       print(data)
    end
end

local buffer = '';
uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1)
-- Set ready to receive uart data
gpio.write(1, gpio.HIGH)
uart.on("data", 0, function(data)
    data:gsub(".", function(c)
        if c == '\n' then
            process_data(buffer)
            buffer = ''
        else
            buffer = buffer..c
        end
    end)
end, 0)

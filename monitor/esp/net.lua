
local function connect(opt, cb)
    local conn = net.createConnection(net.TCP, 0)
    conn:connect(opt.port, opt.host)
    conn:on("receive",function(conn,payload)
        print(payload)
    end)
    conn:on("connection", cb)
    conn:on("disconnection", function(conn, payload)
        print(payload)
        collectgarbage()
    end)
    return conn
end

function send_level(metric, level)
    local ts = rtctime.get();
    connect(CONFIG.MONITOR, function(sock)
        local str = string.format("monitor.%s %s %d", metric, level, ts);
        sock:on('sent', function()
            print("Sent '" .. str .. "'")
            blink(1, 50)
            sock:close()
            collectgarbage()
        end)
        sock:send(str .. "\r\n")
    end)
end

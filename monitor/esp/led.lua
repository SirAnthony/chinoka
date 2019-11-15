
local led_pin = 4
gpio.mode(led_pin, gpio.OUTPUT)

local function blink_cb(timer, data)
    local st = gpio.read(led_pin)
    if st==gpio.LOW then st = gpio.HIGH else st = gpio.LOW end
    gpio.write(led_pin, st)
    data.count = data.count-1
    if data.count<=0 then
        timer:unregister()
        gpio.write(led_pin, gpio.HIGH)
    end
end

function blink(count, interval)
    local data = {count = count*2, interval = interval}
    tmr.create():alarm(interval, tmr.ALARM_AUTO, function(t)
        blink_cb(t, data) end)
end

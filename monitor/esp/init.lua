dofile("config.lua")

-- Times the station will attempt to connect to the AP. Should consider AP reboot duration.
CONNECT_TRIES=20
-- Set not ready to Receive uart data
gpio.mode(1, gpio.OUTPUT)
gpio.write(1, gpio.LOW)
dofile("led.lua")

function startup()
    sntp.sync("0.pool.ntp.org", function(sec, usec, server, info)
        print('NTP sync', sec, usec, server)
    end, nil, 1)
    if file.open("init.lua") == nil then
        print("init.lua deleted or renamed")
    else
        print("Running")
        file.close("init.lua")
        dofile("bus.lua")
        print("Waiting for incoming uart data")
        blink(2, 250)
    end
end

-- Define WiFi station event callbacks
local function wifi_connect_event(T)
  print("Connection to AP("..T.SSID..") established!")
  print("Waiting for IP address...")
  if disconnect_ct ~= nil then disconnect_ct = nil end
end

local function wifi_got_ip_event(T)
  -- Note: Having an IP address does not mean there is internet access!
  -- Internet connectivity can be determined with net.dns.resolve().
  print("Wifi connection is ready! IP address is: "..T.IP)
  print("Startup will resume momentarily, you have 3 seconds to abort.")
  print("Waiting...")
  blink(3, 150)
  tmr.create():alarm(3000, tmr.ALARM_SINGLE, startup)
end

local function wifi_disconnect_event(T)
  if T.reason == wifi.eventmon.reason.ASSOC_LEAVE then
    --the station has disassociated from a previously connected AP
    return
  end
  print("\nWiFi connection to AP("..T.SSID..") has failed!")

  --There are many possible disconnect reasons, the following iterates through
  --the list and returns the string corresponding to the disconnect reason.
  for key,val in pairs(wifi.eventmon.reason) do
    if val == T.reason then
      print("Disconnect reason: "..val.."("..key..")")
      break
    end
  end

  if disconnect_ct == nil then
    disconnect_ct = 1
  else
    disconnect_ct = disconnect_ct + 1
  end
  if disconnect_ct < CONNECT_TRIES then
    print("Retrying connection...(attempt "..(disconnect_ct+1)
        .." of "..CONNECT_TRIES..")")
  else
    wifi.sta.disconnect()
    print("Aborting connection to AP!")
    disconnect_ct = nil
  end
end

-- Register WiFi Station event callbacks
wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, wifi_connect_event)
wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, wifi_got_ip_event)
wifi.eventmon.register(wifi.eventmon.STA_DISCONNECTED, wifi_disconnect_event)

blink(1, 100)
print("Connecting to WiFi access point...")
wifi.setmode(wifi.STATION)
wifi.sta.config(CONFIG.WIFI)
-- wifi.sta.connect() not necessary because config() uses auto-connect=true by default

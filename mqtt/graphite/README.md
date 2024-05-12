
mqtt2graphite
=============

MQTT proxy to send data to graphite.
Change `config.json` for basic configuration, `map.json` for mqtt
subscriptions setup. Run by:

```$ ./gate.py```


Map file
--------

JSON file with setups for mqtt routes as object.
Each key represent channel name. MQTT wildcards (#) are allowed.
Value can be string representing payload type, or object in case
of additional config required.

type

> Payload types:
> - n - payload contains a number
> - j - payload is JSON. Extract all keys with numeric values

#### Optional options:

topic

> The MQTT topic is to be mapped to this key in Carbon (Graphite)
> Periods used as separators. If not specified, the MQTT topic name
> will be used (slashes replaced with dots). Hash (#) character will
> be replaced by the MQTT topic name.

Config
------

Basic server configuration.
- cid - proxy instance id
- timeout - reconnect timeout in seconds
- mqtt - settings for mqtt server
- graphite - settings for graphite server

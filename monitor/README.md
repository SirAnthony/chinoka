# monitor
stm32-based (blue pill) monitor of environment

Components
----------

- dht22: temperature, humidity
- bmp280 (bme280): temperature, pressure, (humidity)
- mq4 (mqx): co, co2, ch4, other carbon based gases
- esp8266 (nodemcu, lua): data send

stm-esp UART protocol
---------------------

All meaningful commands start with `>`

- send

```
 >s type data...
```

- cmd

```
 >c command args...
```

All other will be printed to stdio

Notes:
-------------

- MQx additional tuning required. This sensor can be used only for
 comparative use, do not trust absolute numbers much. But in long comparison
 basic curve is same as mhz-19 co2 sensor (whereas mq4 should calculate
 ch4 concentration, not co2). So after sensor is tuned, it can be used even
 for precise measurements.


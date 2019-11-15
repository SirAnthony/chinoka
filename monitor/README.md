# monitor
stm32-based (blue pill) monitor of environment

Components
----------

- bmp280 (bme280): temperature, pressure, (humidity)
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
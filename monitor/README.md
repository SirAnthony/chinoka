# monitor
stm32-based (blue pill) monitor of environment

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
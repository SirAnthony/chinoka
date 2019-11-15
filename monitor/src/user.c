
#include "user.h"
#include "timer.h"
#include "modules/led.h"
#include "modules/bmp280.h"
#include <stdio.h>

void print_metric(const char *metric, double value)
{
  printf(">s metric %s %f\n", metric, value);
}

int strcmp(const unsigned char *s1, const char *s2)
{
  const unsigned char *c1 = (const unsigned char *)s1;
  const unsigned char *c2 = (const unsigned char *)s2;
  unsigned char ch;
  int d = 0;

  while (1) {
    d = (int)(ch = *c1++) - (int)*c2++;
    if (d || !ch)
      break;
  }
  return d;
}

void User_Init()
{
  printf("Begin modules init\n");
  TimerInit();
  Led_Init();
  HAL_Delay(Led_blink(2, 200));
  BMP_Init();
}

uint32_t last_tick = 0;
void User_Loop(uint32_t tick)
{
  if (last_tick+1000<tick)
  {
    last_tick = tick;
    Led_blink(1, 100);
  }
  BMP_Loop(tick);
}




#include "user.h"
#include "timer.h"
#include "modules/led.h"
#include "modules/bmp280.h"

void User_Init()
{
  TimerInit();
  Led_Init();
  HAL_Delay(Led_blink(2, 200));
  BMP_Init();
}

void User_Loop(uint32_t tick)
{
  Led_blink(1, 500);
  BMP_Loop(tick);
}



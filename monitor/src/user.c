
#include "user.h"
#include "timer.h"
#include "modules/led.h"
#include "modules/bmp280.h"
#include "modules/dht22.h"
#include "modules/mq.h"
#include <stdio.h>

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

int uart_print_disable;
unsigned char rx_buffer[3];
extern UART_HandleTypeDef huart1;

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
#if defined(HAL_UART_MODULE_ENABLED) && defined(UART_PRINTF)
  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_RESET)
    return ch;
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
#endif /* UART_PRINTF */
  return ch;
}

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
  DHT_Init();
  MQ_Init();
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
  DHT_Loop(tick);
  MQ_Loop(tick);
}



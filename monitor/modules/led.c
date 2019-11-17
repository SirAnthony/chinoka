/*
 * led.c
 *
 */

#include "main.h"
#include "timer.h"
#include "led.h"

typedef enum {
  wait_on = 0,
  wait_off,
  wait_cycle,
  done,
} Led_States;

typedef struct {
  int delay;
  int count;
  int repeat;
  Led_States state;
} Led_State;

GPIO_InitTypeDef builtin_led;
Led_State state;

void Led_Init()
{
  builtin_led.Mode = GPIO_MODE_OUTPUT_PP;
  builtin_led.Speed = GPIO_SPEED_FREQ_LOW;
  builtin_led.Pin = GPIO_PIN_13;
  builtin_led.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &builtin_led);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void led_cycle(int repeat)
{
  switch (state.state){
  case done:
    return;
  case wait_on:
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    break;
  case wait_off:
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    break;
  case wait_cycle:
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  }
  state.count++;
  if (!repeat)
      state.state = done;
}

int Led_blink(int count, int delay)
{
  state.state = wait_cycle;
  state.repeat = count*2;
  state.delay = delay;
  // Reset led
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  TimerSetup(TIMER_TIM2, &led_cycle, state.delay, state.repeat);
  return state.repeat*state.delay;
}


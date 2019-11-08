/*
 * led.c
 *
 */

#include "main.h"
#include "led.h"

typedef enum {
  idle = 0,
  wait_on,
  wait_off,
  wait_cycle,
  done,
} Led_States;

typedef struct {
  int last_tick;
  int next_tick;
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
}

void Led_Cycle(int ts)
{
  state.last_tick = ts;
  if (state.last_tick<state.next_tick)
    return;
  switch (state.state){
  case done:
    state.state = idle;
    // no break
  case idle:
    return;
  case wait_on:
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    break;
  case wait_off:
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    break;
  case wait_cycle:
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    state.count++;
    if (state.count > state.repeat)
    {
      state.state = done;
      return;
    }
    state.next_tick = state.next_tick+state.delay;
  }
}

void Led_blink(int count, int delay){
  state.state = wait_cycle;
  state.repeat = count*2;
  state.delay = delay;
  state.next_tick = state.last_tick+delay;
}


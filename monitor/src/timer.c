
#include "timer.h"
#include "main.h"

typedef struct {
  TIM_HandleTypeDef instance;
  timer_cb callback;
  int repeat;
} TIM_Instance;

TIM_Instance timers[TIMER_TIM_END];

int timer_init_default(TIM_Instance *tim)
{
  tim->callback = NULL;
  tim->repeat = 0;
  // Period: (TIM2CLK/1000) - 1: 1/1000 s time base.
  tim->instance.Init.Period = 1000U-1U;
  tim->instance.Init.Prescaler = HAL_RCC_GetPCLK1Freq() / 1000U - 1U;
  tim->instance.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim->instance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim->instance.Init.RepetitionCounter = 0;
  return HAL_TIM_Base_Init(&tim->instance);
}

void TIM2_IRQHandler()
{
  HAL_TIM_IRQHandler(&timers[TIMER_TIM2].instance);
}

/* void TIM3_IRQHandler()
{
  HAL_TIM_IRQHandler(&timers[TIMER_TIM3].instance);
} */

void TimerStop(TIM_Instance *tim, int clear)
{
  HAL_TIM_Base_Stop_IT(&tim->instance);
  if (clear)
    tim->callback = NULL;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  TIM_Instance *tim = NULL;
  for (int i=0; i<TIMER_TIM_END; i++)
  {
    if (htim==&timers[i].instance)
    {
      tim = &timers[i];
      break;
    }
  }
  if (!tim)
    return;
  if (tim->repeat>0)
    tim->repeat--;
  if (tim->callback)
    tim->callback(tim->repeat);
  if (!tim->repeat)
    TimerStop(tim, 1);
}

void TimerInit()
{
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  __HAL_RCC_TIM2_CLK_ENABLE();
  // HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(TIM3_IRQn);
  __HAL_RCC_TIM3_CLK_ENABLE();

  timers[TIMER_TIM2].instance.Instance = TIM2;
  // timers[TIMER_TIM3].instance.Instance = TIM3;

  for (int i=0; i<TIMER_TIM_END; i++)
    timer_init_default(&timers[i]);
}

void TimerStart(TIM_Instance *tim)
{
  HAL_TIM_Base_Start_IT(&tim->instance);
}

void TimerReset(TIM_Instance *tim)
{
  __HAL_TIM_SET_COUNTER(&tim->instance, 0);
}

void TimerSetup(Timer_ID id, timer_cb cb, int period, int repeat)
{
  TIM_Instance *tim = &timers[id];
  TimerStop(tim, 1);
  tim->callback = cb;
  tim->repeat = repeat;
  if (tim->instance.Init.Period!=period)
    __HAL_TIM_SET_AUTORELOAD(&tim->instance, period);
  TimerReset(tim);
  TimerStart(tim);
}

void TimerSetupRepeat(timer_cb cb, int period){
  TimerInit(cb, period, -1);
}


#include "timer.h"
#include "main.h"

static timer_cb callback;

static int tim_repeat = -1;
static TIM_HandleTypeDef tim_instance = {
    .Instance = TIM2
};

void TIM2_IRQHandler()
{
  HAL_TIM_IRQHandler(&tim_instance);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim!=&tim_instance)
    return;
  if (tim_repeat>0)
    tim_repeat--;
  if (callback)
      callback(tim_repeat);
  if (!tim_repeat)
    TimerStop(1);
}

int TimerInit()
{
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  __HAL_RCC_TIM2_CLK_ENABLE();

  // Period: (TIM2CLK/1000) - 1: 1/1000 s time base.
  tim_instance.Init.Period = 1000U-1U;
  tim_instance.Init.Prescaler = HAL_RCC_GetPCLK1Freq() / 1000U - 1U;
  tim_instance.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim_instance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim_instance.Init.RepetitionCounter = 0;
  return HAL_TIM_Base_Init(&tim_instance);
}

void TimerStart()
{
  HAL_TIM_Base_Start_IT(&tim_instance);
}

void TimerStop(int clear)
{
  HAL_TIM_Base_Stop_IT(&tim_instance);
  if (clear)
    callback = NULL;
}

void TimerReset()
{
  __HAL_TIM_SET_COUNTER(&tim_instance, 0);
}

void TimerSetup(timer_cb cb, int period, int repeat)
{
  TimerStop(1);
  callback = cb;
  tim_repeat = repeat;
  if (tim_instance.Init.Period!=period)
    __HAL_TIM_SET_AUTORELOAD(&tim_instance, period);
  TimerReset();
  TimerStart();
}

void TimerSetupRepeat(timer_cb cb, int period){
  TimerInit(cb, period, -1);
}

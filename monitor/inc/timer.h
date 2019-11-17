#ifndef TIMER_H_
#define TIMER_H_

typedef void (*timer_cb)(int repeats_left);

typedef enum {
  TIMER_TIM2 = 0,
  // TIMER_TIM3,
  TIMER_TIM_END,
} Timer_ID;

void TimerInit();

//void TimerStart();
//void TimerReset();
//void TimerStop(int clear);
void TimerSetup(Timer_ID id, timer_cb cb, int period, int repeat);
//void TimerSetupRepeat(Timer_ID id, timer_cb cb, int period);

#endif /* TIMER_H_ */

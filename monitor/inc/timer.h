#ifndef TIMER_H_
#define TIMER_H_

typedef void (*timer_cb)(int repeats_left);

int TimerInit();
void TimerStart();
void TimerReset();
void TimerStop(int clear);
void TimerSetup(timer_cb cb, int period, int repeat);
void TimerSetupRepeat(timer_cb cb, int period);

#endif /* TIMER_H_ */

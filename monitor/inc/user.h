#ifndef USER_H_
#define USER_H_

#include "main.h"

#define UART_PRINTF 1
void print_metric(const char *metric, double value);
int strcmp(const unsigned char *s1, const char *s2);

typedef enum {
  MOD_STATE_IDLE = 0,
  MOD_STATE_WAIT,
  MOD_STATE_RUN,
  MOD_STATE_DELAY,
  MOD_STATE_END,
} ModuleState;

void User_Init();
void User_Loop(uint32_t tick);

#endif /* USER_H_ */

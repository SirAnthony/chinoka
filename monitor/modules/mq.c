
#include "main.h"
#include "user.h"
#include "define.h"
#include "mq.h"

typedef struct {
  volatile float ppm;
  volatile float read;
  ModuleState state;
  uint32_t tick;
} MQxData;

MQxData sdata_mq;

extern ADC_HandleTypeDef hadc1;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hdl)
{
  if (hdl == &hadc1)
  {
      sdata_mq.read = HAL_ADC_GetValue(&hadc1);
      sdata_mq.state = MOD_STATE_RUN;
  }
}

void MQ_Init()
{
  sdata_mq.tick = 0;
  sdata_mq.state = MOD_STATE_DELAY;
}

void MQ_Loop(uint32_t tick)
{
  switch (sdata_mq.state)
  {
  case MOD_STATE_IDLE:
    sdata_mq.tick = tick;
    sdata_mq.state = MOD_STATE_DELAY;
    break;
  case MOD_STATE_DELAY:
    if (sdata_mq.tick<=tick)
    {
      sdata_mq.state = MOD_STATE_WAIT;
      HAL_ADC_Start_IT(&hadc1);
    }
    break;
  case MOD_STATE_WAIT:
    if (sdata_mq.tick>tick)
      return;
    sdata_mq.tick = tick + MQ_MOD_DELAY/2;
    break;
  case MOD_STATE_RUN:
    sdata_mq.tick = tick + MQ_MOD_DELAY;
    sdata_mq.state = MOD_STATE_DELAY;
    // I have no idea how it should be interpreted
    sdata_mq.ppm = ( sdata_mq.read / 4096.0 ) * 3300.0;
    if (sdata_mq.ppm>200)
      print_metric("mq4.co", sdata_mq.ppm);
    break;
  case MOD_STATE_END:
    break;
  }
}


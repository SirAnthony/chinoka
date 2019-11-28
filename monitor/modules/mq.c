
#include "main.h"
#include "user.h"
#include "define.h"
#include "mq.h"
#include <math.h>

// Some info about mq sensors:
// http://davidegironi.blogspot.com/2014/01/cheap-co2-meter-using-mq135-sensor-with.html
// http://sandboxelectronics.com/?p=165
float COCurve[3] = {2.3, 0.72, -0.34}; // mq2 data from above

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
  printf("MQ module init\n");
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
    // 4096  - stm32 has a 12 bit ADC
    //float voltage = ( 4096.0 - sdata_mq.read ) / sdata_mq.read * MQ_RL_VAL;
    //float rs = ( ( 5.0 - voltage ) / voltage ) * MQ_RL_VAL;
    double rs = ( 4096.0 - sdata_mq.read ) / sdata_mq.read * MQ_RL_VAL;
    double ratio = rs / MQ_R0_VAL;
    sdata_mq.ppm = pow(10, (log10(ratio) - COCurve[1]) / COCurve[2] + COCurve[0]);
    if (sdata_mq.ppm>200)
      print_metric("mq4.co", sdata_mq.ppm);
    break;
  case MOD_STATE_END:
    break;
  }
}


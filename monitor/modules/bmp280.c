
#include "main.h"
#include "define.h"
#include "user.h"
#include "bmp280.h"
#include "BMP280_STM32/BMP280/bmp280.h"
#include "stdio.h"

extern I2C_HandleTypeDef hi2c1;

typedef struct {
  float pressure;
  float temperature;
  float humidity;
  BMxSensorType type;
  ModuleState state;
  uint32_t tick;
  BMP280_HandleTypedef handle;
} BMxData;

BMxData sdata_bmp;
uint32_t last_tick;

void BMP_Init()
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BMP_LD4_Pin|BMP_LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BMP_B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BMP_B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = BMP_LD4_Pin|BMP_LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  bmp280_init_default_params(&sdata_bmp.handle.params);
  sdata_bmp.handle.addr = BMP280_I2C_ADDRESS_0;
  sdata_bmp.handle.i2c = &hi2c1;
  sdata_bmp.state = MOD_STATE_IDLE;

  while (!bmp280_init(&sdata_bmp.handle, &sdata_bmp.handle.params)) {
    printf("BMP280 initialization failed\n");
    HAL_Delay(2000);
  }
  sdata_bmp.type = sdata_bmp.handle.id == BME280_CHIP_ID ? TYPE_BME : TYPE_BMP;
  printf("BMP280: found %s\n", sdata_bmp.type==TYPE_BME ? "BME280" : "BMP280");
}

void BMP_Loop(uint32_t tick)
{
  int ret = 0;
  switch (sdata_bmp.state)
  {
  case MOD_STATE_IDLE:
    sdata_bmp.tick = tick;
    sdata_bmp.state = MOD_STATE_RUN;
    break;
  case MOD_STATE_DELAY:
    if (sdata_bmp.tick<=tick)
      sdata_bmp.state = MOD_STATE_RUN;
    break;
  case MOD_STATE_RUN:
    ret = bmp280_read_float(&sdata_bmp.handle, &sdata_bmp.temperature,
          &sdata_bmp.pressure, &sdata_bmp.humidity);
    sdata_bmp.tick = tick + BMP_MOD_DELAY;
    sdata_bmp.state = MOD_STATE_DELAY;
    if (!ret)
    {
      printf("BMx280 Temperature/pressure reading failed\n");
      return;
    }
    print_metric("bmp280.pressure", sdata_bmp.pressure);
    print_metric("bmp280.temperature", sdata_bmp.temperature);
    if (sdata_bmp.type == TYPE_BME)
      print_metric("bmp280.humidity", sdata_bmp.humidity);
    break;
  case MOD_STATE_WAIT:
  case MOD_STATE_END:
    break;
  }
}


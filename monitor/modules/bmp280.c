
#include "main.h"
#include "bmp280.h"
#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "BMP280_STM32/BMP280/bmp280.h"
#include "stdio.h"

#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define LD4_Pin GPIO_PIN_8
#define LD4_GPIO_Port GPIOC
#define LD3_Pin GPIO_PIN_9
#define LD3_GPIO_Port GPIOC

extern I2C_HandleTypeDef hi2c1;

typedef struct {
  float pressure;
  float temperature;
  float humidity;
  BMxSensorType type;
  BMP280_HandleTypedef handle;
} BMxData;

BMxData sdata;

void BMP_Init()
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  bmp280_init_default_params(&sdata.handle.params);
  sdata.handle.addr = BMP280_I2C_ADDRESS_0;
  sdata.handle.i2c = &hi2c1;

  while (!bmp280_init(&sdata.handle, &sdata.handle.params)) {
    printf("BMP280 initialization failed\n");
    HAL_Delay(2000);
  }
  sdata.type = sdata.handle.id == BME280_CHIP_ID ? TYPE_BME : TYPE_BMP;
  printf("BMP280: found %s\n", sdata.type==TYPE_BME ? "BME280" : "BMP280");
}

void BMP_Loop(uint32_t tick)
{
  HAL_Delay(100);
  while (!bmp280_read_float(&sdata.handle, &sdata.temperature,
      &sdata.pressure, &sdata.humidity)) {
    printf("Temperature/pressure reading failed\n");
    HAL_Delay(2000);
  }

  printf("Pressure: %.2f Pa, Temperature: %.2f C", sdata.pressure, sdata.temperature);
  if (sdata.type == TYPE_BME)
    printf(", Humidity: %.2f\n", sdata.humidity);
  else
    printf("\n");
  HAL_Delay(2000);
}



#include "main.h"
#include "define.h"
#include "user.h"
#include "dht22.h"
#include <stdio.h>

#define CHECK_PIN_SET (HAL_GPIO_ReadPin(DHT_GPIO_PORT, DHT_GPIO_PIN) == GPIO_PIN_SET)
#define CHECK_PIN_RESET (HAL_GPIO_ReadPin(DHT_GPIO_PORT, DHT_GPIO_PIN) == GPIO_PIN_RESET)

typedef struct {
  volatile float temperature;
  volatile float humidity;
  uint8_t parity;
  uint8_t parity_rcv;
  uint8_t hMSB;
  uint8_t hLSB;
  uint8_t tMSB;
  uint8_t tLSB;
  uint8_t bits[40];
  ModuleState state;
  uint32_t tick;
} DHTxData;

DHTxData sdata_dht;

void DHT_GPIO_Switch(uint32_t mode, uint32_t pull)
{
  GPIO_InitTypeDef gpio_struct;
  gpio_struct.Pin = DHT_GPIO_PIN;
  gpio_struct.Mode = mode;
  gpio_struct.Pull = pull;
  gpio_struct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT_GPIO_PORT, &gpio_struct);
}

void DHT_Init(void)
{
  TIM_HandleTypeDef tim_struct;
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  tim_struct.Instance = DHT_DELAY_TIM;
  tim_struct.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim_struct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim_struct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  tim_struct.Init.Period = 65535;
  tim_struct.Init.Prescaler = (SystemCoreClock / 1000000U) - 1;
  if (HAL_TIM_Base_Init(&tim_struct) != HAL_OK)
    Error_Handler();
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tim_struct, &sClockSourceConfig) != HAL_OK)
    Error_Handler();
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim_struct, &sMasterConfig) != HAL_OK)
    Error_Handler();
  HAL_TIM_Base_Start(&tim_struct);
  DHT_GPIO_Switch(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
  HAL_GPIO_WritePin(DHT_GPIO_PORT, DHT_GPIO_PIN, GPIO_PIN_RESET);
  printf("DHT module init\n");
}

uint32_t DHT22_GetReadings(DHTxData *out) {
  uint8_t i;
  uint16_t c;

  // Switch pin to output
  DHT_GPIO_Switch(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
  HAL_GPIO_WritePin(DHT_GPIO_PORT, DHT_GPIO_PIN, GPIO_PIN_RESET);
  HAL_Delay(2);

  // Generate start impulse
  HAL_GPIO_WritePin(DHT_GPIO_PORT, DHT_GPIO_PIN, GPIO_PIN_SET);


  // Switch pin to input with Pull-Up
  DHT_GPIO_Switch(GPIO_MODE_INPUT, GPIO_NOPULL);

  // Wait for AM2302 to begin communication (20-40us)
  DHT_DELAY_TIM->CNT = 0;
  while (((c = DHT_DELAY_TIM->CNT) < 1000) && CHECK_PIN_SET);
  DHT_DELAY_TIM->CNT = 0;
  if (c >= 100)
  {
    printf("DHT22_RCV_NO_RESPONSE DELAY_US_TIM->CNT = %d \n", c);
    return DHT22_RCV_NO_RESPONSE;
  }

  // Check ACK strobe from sensor
  while (((c = DHT_DELAY_TIM->CNT) < 100) && CHECK_PIN_RESET);
  DHT_DELAY_TIM->CNT = 0;

  if ((c < 65) || (c > 95))
  {
    printf("DHT22_RCV_BAD_ACK1 DELAY_US_TIM->CNT = %d \n", c);
    return DHT22_RCV_BAD_ACK1;
  }

  while (((c = DHT_DELAY_TIM->CNT) < 100) && CHECK_PIN_SET);
  DHT_DELAY_TIM->CNT = 0;
  if ((c < 65) || (c > 95))
  {
    printf("DHT22_RCV_BAD_ACK2 DELAY_US_TIM->CNT = %d \n", c);
    return DHT22_RCV_BAD_ACK2;
  }


  // ACK strobe received --> receive 40 bits
  for (i = 0; i < 40; i++)
  {
    // Measure bit start impulse (T_low = 50us)

    while (((c = DHT_DELAY_TIM->CNT) < 100) && CHECK_PIN_RESET);
    if (c > 75)
    {
      // invalid bit start impulse length
      out->bits[i] = 0xff;
      while (((c = DHT_DELAY_TIM->CNT) < 100) && CHECK_PIN_SET);
      DHT_DELAY_TIM->CNT = 0;
    }
    else
    {
      // Measure bit impulse length (T_h0 = 25us, T_h1 = 70us)
      DHT_DELAY_TIM->CNT = 0;
      while (((c = DHT_DELAY_TIM->CNT) < 100) && CHECK_PIN_SET);
      DHT_DELAY_TIM->CNT = 0;
      out->bits[i] = (c < 100) ? (uint8_t) c : 0xff;
    }
  }

  for (i = 0; i < 40; i++)
  {
    if (out->bits[i] == 0xff)
      return DHT22_RCV_TIMEOUT;
  }

  return DHT22_RCV_OK;
}

uint16_t DHT22_DecodeReadings(DHTxData *out) {
  uint8_t  i = 0;
  uint16_t temp;

  for (; i < 8; i++) {
    out->hMSB <<= 1;
    if (out->bits[i] > 48)
      out->hMSB |= 1;
  }
  for (; i < 16; i++) {
    out->hLSB <<= 1;
    if (out->bits[i] > 48)
      out->hLSB |= 1;
  }
  for (; i < 24; i++) {
    out->tMSB <<= 1;
    if (out->bits[i] > 48)
      out->tMSB |= 1;
  }
  for (; i < 32; i++) {
    out->tLSB <<= 1;
    if (out->bits[i] > 48)
      out->tLSB |= 1;
  }
  for (; i < 40; i++) {
    out->parity_rcv <<= 1;
    if (out->bits[i] > 48)
      out->parity_rcv |= 1;
  }

  out->parity  = out->hMSB + out->hLSB + out->tMSB + out->tLSB;
  sdata_dht.humidity = ((out->hMSB << 8) | out->hLSB) / 10.0f;

  temp = (out->tMSB << 8) | out->tLSB;
  sdata_dht.temperature = (temp & 0x7fff) / 10.0f;
  if (temp & 0x8000)
    sdata_dht.temperature = -sdata_dht.temperature;

  if (out->parity_rcv != out->parity)
    return DHT22_RCV_BAD_DATA;
  return DHT22_RCV_OK;
}

void DHT_Loop(uint32_t tick)
{
  uint8_t ret = 0;
  switch (sdata_dht.state)
  {
  case MOD_STATE_IDLE:
    sdata_dht.tick = tick;
    sdata_dht.state = MOD_STATE_RUN;
    break;
  case MOD_STATE_DELAY:
    if (sdata_dht.tick<=tick)
      sdata_dht.state = MOD_STATE_RUN;
    break;
  case MOD_STATE_RUN:
    ret = DHT22_GetReadings(&sdata_dht);
    sdata_dht.tick = tick + DHT_MOD_DELAY;
    sdata_dht.state = MOD_STATE_DELAY;
    if (ret != DHT22_RCV_OK)
    {
      printf("DHT22 reading failed\n");
      return;
    }
    ret = DHT22_DecodeReadings(&sdata_dht);
    if (ret != DHT22_RCV_OK)
      printf("DHT22 wrong data received");
    else
    {
      print_metric("dht22.temperature", sdata_dht.temperature);
      print_metric("dht22.humidity", sdata_dht.humidity);
    }
    break;
  case MOD_STATE_WAIT:
  case MOD_STATE_END:
    break;
  }
}

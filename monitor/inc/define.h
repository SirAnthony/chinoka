#ifndef DEFINE_H_
#define DEFINE_H_

// BMP
#define BMP_B1_Pin         GPIO_PIN_0
#define BMP_B1_GPIO_Port   GPIOA
#define BMP_LD4_Pin        GPIO_PIN_8
#define BMP_LD4_GPIO_Port  GPIOC
#define BMP_LD3_Pin        GPIO_PIN_9
#define BMP_LD3_GPIO_Port  GPIOC
#define BMP_MOD_DELAY      5000U

// DHT
#define DHT_GPIO_PORT    GPIOB
// #define DHT_GPIO_CLOCK   RCC_APB2Periph_GPIOB
#define DHT_DELAY_TIM    TIM3
#define DHT_GPIO_PIN     GPIO_PIN_8
#define DHT_MOD_DELAY    2000U

// MQ
#define MQ_GPIO_PIN     A1
// Board resistance
#define MQ_RL_VAL       15
//RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO
#define MQ_R0_VAL       9.3
#define MQ_ZERO_VAL     200
#define MQ_MOD_DELAY    10000U

// MHZ
#define MHZ_UART 1

#endif /* DEFINE_H_ */

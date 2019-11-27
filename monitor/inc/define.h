#ifndef DEFINE_H_
#define DEFINE_H_

// BMP
#define BMP_B1_Pin         GPIO_PIN_0
#define BMP_B1_GPIO_Port   GPIOA
#define BMP_LD4_Pin        GPIO_PIN_8
#define BMP_LD4_GPIO_Port  GPIOC
#define BMP_LD3_Pin        GPIO_PIN_9
#define BMP_LD3_GPIO_Port  GPIOC
#define BMP_MOD_DELAY      2000U

// DHT
#define DHT_GPIO_PORT    GPIOA
// #define DHT_GPIO_CLOCK   RCC_APB2Periph_GPIOB
#define DHT_DELAY_TIM    TIM3
#define DHT_GPIO_PIN     GPIO_PIN_15
#define DHT_MOD_DELAY    2000U

// MQ
#define MQ_GPIO_PIN     A1
#define MQ_ZERO_VAL     200
#define MQ_MOD_DELAY    10000U

#endif /* DEFINE_H_ */

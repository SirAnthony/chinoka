#ifndef BMP280_H_
#define BMP280_H_

#include "main.h"

typedef enum {
  TYPE_BM_NONE = 0,
  TYPE_BMP,
  TYPE_BME,
} BMxSensorType;

void BMP_Init();
void BMP_Loop(uint32_t dt);


#endif /* BMP280_H_ */

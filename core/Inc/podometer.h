#ifndef PODOMETER_H_
#define PODOMETER_H_

#include "stm32g4xx_hal.h"
#include "MPU6050/stm32g4_mpu6050.h"

extern uint32_t global_step_count;

void Podometer_Init(void);
void Podometer_Update(void);

#endif /* PODOMETER_H_ */

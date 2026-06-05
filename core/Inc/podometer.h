#ifndef PODOMETER_H_
#define PODOMETER_H_

/**
 * @file podometer.h
 * @brief Gestion du comptage des pas.
 */
#include "stm32g4xx_hal.h"
#include "MPU6050/stm32g4_mpu6050.h"

/**
 * @brief Nombre total de pas détectés.
 */
extern uint32_t global_step_count;

/**
 * @brief Initialise le podomètre.
 */
void Podometer_Init(void);

/**
 * @brief Met à jour le comptage des pas.
 */
void Podometer_Update(void);

#endif /* PODOMETER_H_ */

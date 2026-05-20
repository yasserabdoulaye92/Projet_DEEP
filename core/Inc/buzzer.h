/**
 * @file    buzzer.h
 * @brief   Prototypes et définitions pour la gestion du buzzer par timer.
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32g4xx_hal.h"
#include "stm32g4_timer.h"

/* Configuration sur la broche PA1 */
#define BUZZER_PIN   GPIO_PIN_1
#define BUZZER_GPIO  GPIOA

/**
 * @brief Initialise la broche GPIO du buzzer et le Timer 6.
 */
void Buzzer_Init(void);

/**
 * @brief Déclenche un bip sonore asynchrone (non-bloquant).
 * @param duration_ms Durée du bip en millisecondes.
 */
void Buzzer_Beep(uint32_t duration_ms);

#endif /* BUZZER_H_ */

/**
 * @file    buzzer.h
 * @author  Yasser Abdoulaye
 * @brief   Pilote buzzer piezo asynchrone via Timer 6 (interruption).
 *
 * @details Le buzzer est piloté en tout-ou-rien sur PA1. La fréquence du son
 *          est déterminée par la période du Timer 6 (500 µs → toggle → 1 kHz).
 *          L'appel à Buzzer_Beep() est **non bloquant** : le bip se termine
 *          automatiquement en ISR sans mobiliser la super-boucle.
 *
 * @section Brochage Brochage matériel
 *  - Signal : PA1 (GPIO_MODE_OUTPUT_PP)
 *  - Timer  : TIMER6 (TIMER6_user_handler_it dans buzzer.c)
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32g4xx_hal.h"
#include "stm32g4_timer.h"

/** @brief Broche de sortie du buzzer. */
#define BUZZER_PIN   GPIO_PIN_1

/** @brief Port GPIO du buzzer. */
#define BUZZER_GPIO  GPIOA

/**
 * @brief  Initialise la broche PA1 en sortie push-pull et configure le
 *         Timer 6 à 500 µs (→ 1 kHz). Le timer est arrêté après init.
 * @note   À appeler une seule fois avant la super-boucle.
 */
void Buzzer_Init(void);

/**
 * @brief  Déclenche un bip sonore **non bloquant** d'une durée donnée.
 * @param  duration_ms  Durée du bip en millisecondes (ex: 200 pour 200 ms).
 * @note   Un appel pendant un bip en cours relance immédiatement un nouveau bip.
 */
void Buzzer_Beep(uint32_t duration_ms);

#endif /* BUZZER_H_ */

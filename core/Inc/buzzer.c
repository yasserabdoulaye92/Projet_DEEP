/**
 * @file    buzzer.c
 * @author  Yasser Abdoulaye
 * @brief   Implémentation du pilote buzzer asynchrone via Timer 6.
 *
 * @details Principe de fonctionnement :
 *  - Buzzer_Beep(N) démarre le Timer 6 et programme N*2 toggles de la broche.
 *  - TIMER6_user_handler_it() s'exécute toutes les 500 µs et bascule PA1.
 *  - Après le dernier toggle, le timer s'arrête et PA1 est forcé à l'état bas.
 *
 *  Calcul de fréquence :
 *    Période timer = 500 µs → 1 toggle toutes les 500 µs → f = 1/(2×500µs) = 1 kHz
 */

#include "buzzer.h"

/** @brief Nombre de toggles déjà effectués depuis le début du bip. */
static volatile uint32_t beep_toggle_count = 0;

/** @brief Nombre total de toggles à effectuer pour la durée demandée. */
static volatile uint32_t beep_toggle_target = 0;

/**
 * @brief  Initialise PA1 en sortie push-pull et prépare le Timer 6.
 */
void Buzzer_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = BUZZER_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_GPIO, &gpio);

    HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);

    /* 500 µs par toggle → 1 kHz (fréquence de résonance typique buzzer piezo) */
    BSP_TIMER_run_us(TIMER6_ID, 500, true);
    BSP_TIMER_stop(TIMER6_ID);
}

/**
 * @brief  Lance un bip non bloquant.
 * @param  duration_ms  Durée souhaitée en millisecondes.
 */
void Buzzer_Beep(uint32_t duration_ms)
{
    beep_toggle_count  = 0;
    /* À 1 kHz : 2 toggles par ms → duration_ms * 2 toggles de 500 µs */
    beep_toggle_target = duration_ms * 2;
    BSP_TIMER_start(TIMER6_ID);
}

/**
 * @brief  ISR du Timer 6 — appelée toutes les 500 µs par le BSP.
 *         Bascule PA1 et arrête le timer quand la durée est atteinte.
 */
void TIMER6_user_handler_it(void)
{
    if (beep_toggle_count < beep_toggle_target) {
        HAL_GPIO_TogglePin(BUZZER_GPIO, BUZZER_PIN);
        beep_toggle_count++;
    } else {
        BSP_TIMER_stop(TIMER6_ID);
        HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);
    }
}

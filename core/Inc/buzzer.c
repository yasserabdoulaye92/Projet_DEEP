/**
 * @file    buzzer.c
 * @brief   Gestion du buzzer en arrière-plan via les interruptions du TIMER6.
 */

#include "buzzer.h"

static volatile uint32_t beep_toggle_count = 0;
static volatile uint32_t beep_toggle_target = 0;

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

    // Configuration de la base de temps (1000 us = 1 ms)
    BSP_TIMER_run_us(TIMER6_ID, 1000, true);
    BSP_TIMER_stop(TIMER6_ID);
}

void Buzzer_Beep(uint32_t duration_ms)
{
    beep_toggle_count  = 0;
    beep_toggle_target = duration_ms * 2; // Fréquence de l'oscillation
    BSP_TIMER_start(TIMER6_ID);
}

/**
 * @brief Handler d'interruption du Timer 6 appelé chaque milliseconde.
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

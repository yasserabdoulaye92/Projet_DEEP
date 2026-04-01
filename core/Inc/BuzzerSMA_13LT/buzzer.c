#include "buzzer.h"

/**
  * @brief Initialisation du GPIO pour le buzzer
  */
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Activer l'horloge du port A
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(BUZZER_GPIO, &GPIO_InitStruct);

    // État initial : éteint
    HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);
}

/**
  * @brief Génère une oscillation audible (environ 500Hz)
  * @param duration_ms: Durée totale du bip
  */
void Buzzer_Tone(uint32_t duration_ms)
{
    uint32_t start_time = HAL_GetTick();

    while((HAL_GetTick() - start_time) < duration_ms)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_SET);
        HAL_Delay(1); // 1ms ON
        HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);
        HAL_Delay(1); // 1ms OFF
    }
}

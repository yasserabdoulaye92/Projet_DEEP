#include "stm32g4xx_hal.h"
#include "config.h"

// Définitions
#define BUZZER_PIN  GPIO_PIN_5
#define BUZZER_GPIO GPIOA

/* Prototypes */
void SystemClock_Config(void); // Définie dans stm32g4_sys.c
static void MX_GPIO_Init(void);
void Buzzer_Tone(uint32_t duration_ms); // On simplifie pour le test

int main(void)
{
    /* 1. Initialisations */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    /* 2. Boucle principale */
    while (1)
    {
        // On appelle la fonction qui existe vraiment en bas
        Buzzer_Tone(200);
        HAL_Delay(800);
    }
}

/**
  * @brief Génère une oscillation de 500Hz (2ms de période)
  * Cette fréquence est lente mais GARANTIE audible sur un piezo passif.
  */
void Buzzer_Tone(uint32_t duration_ms)
{
    uint32_t start_time = HAL_GetTick();

    while((HAL_GetTick() - start_time) < duration_ms)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_SET);
        HAL_Delay(1); // 1ms à l'état haut
        HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);
        HAL_Delay(1); // 1ms à l'état bas
    }
}

/**
  * @brief Initialisation du GPIO PA5
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(BUZZER_GPIO, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);
}

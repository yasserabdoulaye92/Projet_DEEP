#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"

#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>
#include "bpm.h"
#include "ui.h"

char time_msg[32];
char display_msg[32];
int main(void)
{
    // Initialisations
    HAL_Init();
    SystemClock_Config();
    ILI9341_Init();
    BSP_ADC_init();
    while (1)
    {
        // 1. Calcul du BPM
        uint8_t bpm = BPM_Calculate();

        // 2. Formatage du texte dans notre variable display_msg
        sprintf(display_msg, "Pouls: %03d BPM", bpm);

        // 3. Affichage
        ILI9341_Puts(20, 100, display_msg, &Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE);

        // 4. Pause courte
        HAL_Delay(20);
    }
}

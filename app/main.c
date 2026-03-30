#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"

#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>

#include "rtc_manager.h"

/* Les variables globales se déclarent EN DEHORS du main */
char time_msg[32];
int main(void)
{

    HAL_Init();
    SystemClock_Config();
    ILI9341_Init();
    while (1)
    {

        RTC_GetTimeDateString(time_msg);
        ILI9341_Puts(10, 10, time_msg, &Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        HAL_Delay(500);
    }
}

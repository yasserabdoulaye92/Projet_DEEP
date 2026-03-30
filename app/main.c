#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"

#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>

#include "ui.h"

int16_t x_t, y_t;
char msg[32];

int main(void) {
  HAL_Init();
  SystemClock_Config();

  ILI9341_Init();
  XPT2046_init();

  DrawMainMenu();

  // CORRECTION ICI : Font_16x26
  ILI9341_Puts(10, 220, "Touche l'ecran...  ", &Font_16x26, ILI9341_COLOR_GRAY, ILI9341_COLOR_WHITE);

  while (1)
  {
    if (XPT2046_getCoordinates(&x_t, &y_t, XPT2046_COORDINATE_SCREEN_RELATIVE)) {

        sprintf(msg, "OK! X:%03d Y:%03d ", x_t, y_t);

        // CORRECTION ICI : Font_16x26
        ILI9341_Puts(10, 220, msg, &Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_GREEN);

        HAL_Delay(300);

        // CORRECTION ICI : Font_16x26
        ILI9341_Puts(10, 220, "Touche l'ecran...  ", &Font_16x26, ILI9341_COLOR_GRAY, ILI9341_COLOR_WHITE);
    }
  }
}

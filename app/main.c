#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>
#include "menu.h"

/* Prototypes des fonctions système (générées par l'IDE) */
void SystemClock_Config(void);

int main(void) {
HAL_Init();


    // XPT2046_demo();
   //ILI9341_demo();
   //ILI9341_DrawRectangle(20, 20, 60, 60, ILI9341_COLOR_BLUE);
   //ILI9341_Puts(25, 25, "Temperature", &Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_CYAN);
   // ILI9341_DrawRectangle(70, 20, 110, 60, ILI9341_COLOR_BLUE);
   //ILI9341_Puts(75, 25, "Pou", &Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_CYAN);
   // ILI9341_DrawRectangle(120, 20, 160, 60, ILI9341_COLOR_BLUE);
   // ILI9341_Puts(125, 25, "Musique", &Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_CYAN);

    //XPT2046_init();	//initialisation du tactile
	//int16_t x, y;

    //while(1){
		//if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
		//{

			//printf("x: %d\ty:%d\n", x, y);
		//}

   // }

   SystemClock_Config();

    // Initialisation du menu (qui initialise aussi l'écran)
    MENU_init();


    while (1) {
        // On surveille le tactile en permanence
        MENU_handler();

        HAL_Delay(10);
    }
}

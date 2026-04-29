#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>
#include "menu.h"

/* Prototypes des fonctions système (générées par l'IDE) */
void SystemClock_Config(void);

int main(void) {
HAL_Init();
    MENU_init();

    while (1) {
        // On surveille le tactile en permanence
        MENU_handler();


    }
}

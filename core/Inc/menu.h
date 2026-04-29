#ifndef MENU_H_
#define MENU_H_

#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"

typedef enum {
	PAGE_PRINCIPALE = 0,
	PAGE_SANTE,  // Sous-menu pour la case 1
	PAGE_HORLOGE,   // Sous-menu pour la case 2 (etc...)
	PAGE_CARTES,
	PAGE_GAMES,
	PAGE_METEO

} Page_t;

#define NB_ROWS    3
#define NB_COLS    2
#define CELL_W     100
#define CELL_H     60
#define MARGIN_X   15
#define MARGIN_Y   45
#define GAP        10

// Prototypes des fonctions
void MENU_init(void);
void MENU_draw(void);
void MENU_handler(void);

#endif

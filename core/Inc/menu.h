#ifndef MENU_H_
#define MENU_H_

#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"

<<<<<<< Updated upstream
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
=======
// Définition stricte des pages de la montre
typedef enum {
    PAGE_MENU = 0,
    PAGE_HORLOGE,
    PAGE_NOTIF,
    PAGE_SANTE,
    PAGE_METEO,
    PAGE_NFC,
    PAGE_REGLAGES
} Page_t;

// Prototypes des fonctions publiques
>>>>>>> Stashed changes
void MENU_init(void);
void MENU_draw(void);
void MENU_handler(void);

<<<<<<< Updated upstream
#endif
=======
// NOUVEAU : Fonction pour recevoir une notification !
void MENU_set_notif(char* texte);

#endif /* MENU_H_ */
>>>>>>> Stashed changes

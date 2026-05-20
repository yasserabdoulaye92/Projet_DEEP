/**
 * @file    menu.h
 * @brief   Gestion de l'interface graphique LCD et du moteur tactile.
 */

#ifndef MENU_H_
#define MENU_H_

#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"

typedef enum {
    PAGE_MENU = 0,
    PAGE_HORLOGE,
    PAGE_NOTIF,
    PAGE_SANTE,
    PAGE_METEO,
    PAGE_NFC,
    PAGE_REGLAGES
} Page_t;

void MENU_init(void);
void MENU_draw(void);
void MENU_handler(void);
void MENU_update_time(char* time_str);
void MENU_set_notif(char* texte);
void MENU_set_meteo(char* etat, char* temp);

#endif /* MENU_H_ */

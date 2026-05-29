/**
 * @file    menu.h
 * @brief   Gestion de l'interface graphique LCD et du moteur tactile.
 */

#ifndef MENU_H_
#define MENU_H_

#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdbool.h>

/**
 * @enum  Page_t
 * @brief Énumération des différentes pages de l'application.
 */
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
void MENU_set_meteo(char* infos);

/**
 * @brief Met à jour le graphique ECG et le rythme cardiaque en temps réel.
 * @param adc_val Valeur brute du capteur HW-827 (0 à 4095)
 * @param bpm Rythme cardiaque calculé
 * @param is_beating Indique si un pic (battement) est détecté à cet instant
 */
void MENU_update_sante(uint16_t adc_val, int bpm, bool is_beating);

#endif /* MENU_H_ */

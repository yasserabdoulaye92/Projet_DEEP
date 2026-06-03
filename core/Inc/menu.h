/**
 * @file    menu.h
 * @brief   Gestionnaire de l'interface graphique (GUI) et du tactile.
 * @details Définit les pages de la montre, les prototypes d'affichage et de navigation.
 */

#ifndef MENU_H_
#define MENU_H_

#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @enum  Page_t
 * @brief Machine à états représentant les différents écrans de l'application.
 */
typedef enum {
    PAGE_MENU = 0,   ///< Menu principal avec les icônes
    PAGE_HORLOGE,    ///< Affichage de l'heure en grand et chronomètre
    PAGE_NOTIF,      ///< Affichage de la dernière notification Bluetooth
    PAGE_SANTE,      ///< Hub Santé (Podomètre & Capteur Cardiaque)
    PAGE_METEO,      ///< Affichage des données météo reçues
    PAGE_NFC,        ///< Page de lecture de badge NFC
    PAGE_REGLAGES    ///< Paramètres (Couleur du thème, Luminosité)
} Page_t;

/* ========================================================================= */
/* PROTOTYPES DES FONCTIONS PUBLIQUES                                        */
/* ========================================================================= */

/**
 * @brief Initialise l'écran TFT, la puce tactile et dessine l'écran d'accueil.
 */
void MENU_init(void);

/**
 * @brief Force le redessin complet de la page actuelle.
 */
void MENU_draw(void);

/**
 * @brief Analyse les appuis tactiles et gère la navigation entre les pages.
 * @note  À appeler en continu dans la boucle infinie (Super-Boucle) du main.
 */
void MENU_handler(void);

/**
 * @brief Met à jour l'heure affichée à l'écran (si elle a changé).
 * @param time_str Chaîne de caractères formatée (ex: "12:30:45").
 */
void MENU_update_time(char* time_str);

/**
 * @brief Stocke et affiche une nouvelle notification texte.
 * @param texte Le message à afficher.
 */
void MENU_set_notif(char* texte);

/**
 * @brief Stocke et affiche les dernières données météorologiques.
 * @param infos Chaîne de caractères formatée (ex: "Nuageux 22C").
 */
void MENU_set_meteo(char* infos);

/**
 * @brief Met à jour dynamiquement l'écran Santé (ECG et Rythme cardiaque).
 * @param adc_val    Valeur brute lue sur la broche analogique (0 à 4095).
 * @param bpm        Rythme cardiaque calculé en battements par minute.
 * @param is_beating Vrai si un pic (battement) est détecté à cet instant.
 */
void MENU_update_sante(uint16_t adc_val, int bpm, bool is_beating);

#endif /* MENU_H_ */

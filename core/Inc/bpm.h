/**
 *******************************************************************************
 * @file    bpm.h
 * @author  Yasser Abdoulaye
 * @brief   En-tête du module de gestion du capteur de pouls HW-827.
 * @details Définit les fonctions pour lire les valeurs analogiques brutes
 * et calculer la fréquence cardiaque en battements par minute (BPM).
 * Utilise la broche PA0 (ADC_1).
 *******************************************************************************
 */

#ifndef BPM_H
#define BPM_H

#include "stm32g4xx_hal.h"
#include <stdint.h>

/**
 * @brief   Lit la valeur tension brute du capteur de pouls.
 * @details Fait appel au convertisseur analogique-numérique (ADC_1) via
 * la librairie BSP du projet pour lire la tension sur la broche PA0.
 * @return  La valeur numérique brute mesurée par l'ADC (généralement entre 0 et 4095).
 */
uint16_t BPM_ReadRawValue(void);

/**
 * @brief   Calcule la fréquence cardiaque en temps réel.
 * @details Analyse les variations de la valeur brute pour détecter les pics
 * (qui correspondent aux battements de coeur) en se basant sur un seuil.
 * Mesure le temps écoulé entre deux pics via HAL_GetTick() pour en déduire le BPM.
 * @return  Le rythme cardiaque actuel en battements par minute (BPM).
 * Retourne 0 si aucun rythme valide n'est détecté.
 */
uint8_t BPM_Calculate(void);

#endif /* BPM_H */

/**
 * @file    rtc_manager.h
 * @author  Yasser
 * @brief   En-tête pour la gestion de l'Horloge Temps Réel (RTC).
 * @date    30 Mars 2026
 */

#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include "stm32g4xx_hal.h" // Nécessaire pour les types HAL
#include <stdint.h>

/**
 * @brief  Récupère l'heure et la date actuelles sous forme de texte.
 * @param  buffer : Tableau de caractères où la chaîne sera stockée (ex: "14:30:00 - 30/03").
 * @retval Aucun.
 */
void RTC_GetTimeDateString(char *buffer);

#endif /* RTC_MANAGER_H */

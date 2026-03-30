/**
 * @file    rtc_manager.c
 * @author  Yasser
 * @brief   Fonctions de gestion de l'Horloge Temps Réel (RTC).
 * @date    30 Mars 2026
 */

#include "rtc_manager.h"
#include <stdio.h>

/* On "importe" la variable hrtc qui est générée automatiquement par CubeMX dans le main.c */
extern RTC_HandleTypeDef hrtc;

/**
 * @brief  Récupère l'heure et la date actuelles sous forme de texte.
 * @note   HAL_RTC_GetDate DOIT être appelée après HAL_RTC_GetTime pour débloquer le registre.
 * @param  buffer : Tableau de caractères où la chaîne sera formatée.
 * @retval Aucun.
 */
void RTC_GetTimeDateString(char *buffer) {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // 1. On lit l'heure d'abord
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    // 2. On lit la date obligatoirement ensuite !
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    //formatage
    sprintf(buffer, "%02d:%02d:%02d  %02d/%02d/20%02d",
            sTime.Hours, sTime.Minutes, sTime.Seconds,
            sDate.Date, sDate.Month, sDate.Year);
}

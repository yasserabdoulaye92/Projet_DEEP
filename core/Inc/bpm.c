/**
 *******************************************************************************
 * @file    bpm.c
 * @author  Yasser Abdoulaye
 * @brief   Code source du module de gestion du capteur de pouls HW-827.
 *******************************************************************************
 */

#include "bpm.h"
#include "stm32g4_adc.h"

uint16_t BPM_ReadRawValue(void) {

    return BSP_ADC_getValue(ADC_1);
}

uint8_t BPM_Calculate(void) {
    static uint32_t last_beat_time = 0;
    static uint8_t is_above_threshold = 0;
    static uint8_t current_bpm = 0;

    uint16_t raw_value = BPM_ReadRawValue();
   // Seuil de détection d'un battement
    uint16_t threshold = 2500;

    // Si le signal dépasse le seuil : un battement est détecté
    if (raw_value > threshold && is_above_threshold == 0) {
        is_above_threshold = 1;

        uint32_t current_time = HAL_GetTick();
        uint32_t delta_time = current_time - last_beat_time;
        last_beat_time = current_time;

        // Filtrage temporel : on vérifie que le temps est physiquement possible pour un humain
        // (entre 30 et 200 BPM, soit un delta compris entre 300ms et 2000ms)
        if (delta_time > 300 && delta_time < 2000) {
            current_bpm = 60000 / delta_time; // 60000 ms = 1 minute
        }
    }
    // Quand le signal redescend sous le seuil : on déverrouille pour attendre le prochain battement
    else if (raw_value < threshold) {
        is_above_threshold = 0;
    }

    return current_bpm;
}

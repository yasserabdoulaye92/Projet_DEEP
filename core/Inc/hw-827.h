/**
 * @file    hw-827.h
 * @author  Candice
 * @brief   Pilote de détection de pouls pour le capteur HW-827 (KY-039).
 *
 * @details Le capteur HW-827 est un capteur infrarouge de transmission.
 *          Il fournit une tension analogique proportionnelle à l'absorption
 *          de la lumière IR par les vaisseaux sanguins (photopléthysmographie).
 *
 *          L'algorithme de détection est dynamique :
 *          - Filtre de moyenne glissante sur 10 échantillons (lissage).
 *          - Suivi adaptatif du min/max pour s'ajuster à la pression du doigt.
 *          - Détection du front montant (pic > 70 % de l'amplitude).
 *
 * @section Brochage Brochage matériel
 *  - Sortie analogique : PA6 (ADC1_IN3 → ADC_3 dans le BSP)
 *  - VCC : 3.3 V ou 5 V
 *
 * @section Limites Limites acceptées
 *  - BPM_MIN : 30 bpm
 *  - BPM_MAX : 200 bpm
 */

#ifndef HW_827_H_
#define HW_827_H_

#include <stdint.h>
#include <stdbool.h>

/** @brief Fréquence cardiaque minimale acceptable (bpm). */
#define BPM_MIN 30

/** @brief Fréquence cardiaque maximale acceptable (bpm). */
#define BPM_MAX 200

/**
 * @brief  Analyse la valeur brute de l'ADC et détecte un battement cardiaque.
 *
 * @details Doit être appelée régulièrement (toutes les @p delayMsec ms).
 *          Retourne true **une seule fois** par battement (front montant du pic).
 *          Retourne false pendant la descente et le creux de la vague.
 *
 * @param  adcRaw    Valeur 12 bits lue par l'ADC (0 à 4095).
 * @param  delayMsec Intervalle d'appel en ms (utilisé pour la validation interne).
 *
 * @retval true  Un nouveau battement (front montant) vient d'être détecté.
 * @retval false Pas de nouveau battement (creux de vague, doigt absent, bruit).
 */
bool HW827_HeartbeatDetected(uint16_t adcRaw, uint16_t delayMsec);

#endif /* HW_827_H_ */

/**
*******************************************************************************
* @file hw-827.h
* @author candice
* @brief Pilote pour le capteur de pouls infrarouge HW-827 (KY-039)
*******************************************************************************
*/

#ifndef HW_827_H_
#define HW_827_H_

#include <stdint.h>
#include <stdbool.h>


#define BPM_MIN 30 // Fréquence cardiaque minimum acceptable
#define BPM_MAX 200 // Fréquence cardiaque maximum acceptable

/**
* @brief Analyse la valeur brute de l'ADC pour détecter un battement de coeur.
* Cette fonction implémente un algorithme de détection de pic dynamique.
* * @param adcRaw : Valeur lue par l'ADC (0 à 4095 pour un STM32 en 12 bits)
* @param delayMsec : Le temps écoulé (en ms) entre chaque appel de la fonction
* * @retval true : Un nouveau battement (front montant du pic) a été détecté
* @retval false : Rien de nouveau (ou on est dans le creux de la vague)
*/
bool HW827_HeartbeatDetected(uint16_t adcRaw, uint16_t delayMsec);

#endif /* HW_827_H_ */

/**
 * @file    podometer.h
 * @author  Yasser Abdoulaye
 * @brief   Podomètre basé sur le capteur inertiel MPU6050.
 *
 * @details Détecte les pas via une machine à états à hystérésis sur la norme
 *          d'accélération filtrée : comptage au front montant du pic (>0.15G),
 *          réarmement à la redescente (<0.08G). Lecture à 50 Hz (20 ms).
 *          Si le capteur ne répond pas, l'init est retentée toutes les 2 s.
 *
 * @section Brochage Brochage matériel
 *  - VCC du MPU6050 : rail +3V3/+5V (câblé en direct, fil rouge)
 *  - SCL I2C1       : PA15
 *  - SDA I2C1       : PB7
 */

#ifndef PODOMETER_H_
#define PODOMETER_H_

#include "stm32g4xx_hal.h"
#include "MPU6050/stm32g4_mpu6050.h"

/**
 * @brief Compteur global de pas, incrémenté par Podometer_Update().
 *        Accessible en lecture depuis n'importe quel module (extern).
 */
extern uint32_t global_step_count;

/**
 * @brief  Initialise le podomètre : remet à zéro le compteur de pas et
 *         démarre le capteur MPU6050 (alimentation + configuration I2C).
 * @note   Doit être appelée une seule fois avant la super-boucle.
 */
void Podometer_Init(void);

/**
 * @brief  Lit l'accélération, applique un filtre passe-bas et détecte les
 *         pas (comptage au front montant, anti-rebond 300 ms).
 * @note   À appeler en continu dans la super-boucle. Non bloquant : cadence
 *         50 Hz, et retry d'init automatique si le MPU6050 est absent.
 */
void Podometer_Update(void);

/**
 * @brief  Indique si le MPU6050 répond sur le bus I2C (PA15/PB7).
 * @retval true  Capteur détecté — les pas sont comptés.
 * @retval false Capteur absent — vérifier le module et son alimentation.
 */
bool Podometer_IsConnected(void);

/**
 * @brief  Résultat du dernier scan I2C de diagnostic (si capteur absent).
 * @return Adresse 8 bits du premier périphérique répondant sur le bus,
 *         ou -1 si le bus est muet. MPU6050 attendu en 0xD0 ou 0xD2.
 */
int16_t Podometer_GetI2CScan(void);

#endif /* PODOMETER_H_ */

/**
 * @file podometer.c
 * @brief Gestion du comptage des pas.
 *
 * Ce fichier permet de détecter les pas à partir
 * des données de l'accéléromètre MPU6050.
 */

#include "podometer.h"
#include <math.h>

/**
 * @brief Nombre total de pas détectés.
 */
uint32_t global_step_count = 0;
/**
 * @brief Structure contenant les données du MPU6050.
 */
static MPU6050_t mpu; 
/**
 * @brief Instant de la dernière lecture du capteur.
 */
static uint32_t lastReadTick = 0;
/**
 * @brief Instant du dernier pas détecté.
 */
static uint32_t lastStepTime = 0;
/**
 * @brief Indique si un pic d'accélération a été détecté.
 */
static uint8_t peakDetected = 0;

/**
 * @brief Initialise le podomètre.
 *
 * Remet le compteur de pas à zéro et initialise
 * le capteur MPU6050.
 */
void Podometer_Init(void) {
    global_step_count = 0;

    MPU6050_Init(&mpu, NULL, 0, MPU6050_Device_0, MPU6050_Accelerometer_2G, MPU6050_Gyroscope_250s);
}

/**
 * @brief Met à jour le comptage des pas.
 *
 * Lit les données de l'accéléromètre, calcule
 * l'accélération totale et détecte les pas
 * à l'aide de seuils hauts et bas.
 */
void Podometer_Update(void) {
    uint32_t now = HAL_GetTick();

    if ((now - lastReadTick) >= 20) {
        lastReadTick = now;

        MPU6050_ReadAll(&mpu);

        float ax = (float)mpu.Accelerometer_X;
        float ay = (float)mpu.Accelerometer_Y;
        float az = (float)mpu.Accelerometer_Z;

        float mag = sqrtf(ax*ax + ay*ay + az*az);
        float seuil_haut = 22000.0f; 
        float seuil_bas  = 18000.0f; 

        if (mag > seuil_haut) {
            peakDetected = 1;
        }

        if (peakDetected && mag < seuil_bas) {
            if (now - lastStepTime >= 300) { 
                global_step_count++;
                lastStepTime = now;
            }
            peakDetected = 0; 
        }
    }
}

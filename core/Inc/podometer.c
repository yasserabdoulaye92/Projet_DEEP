#include "podometer.h"
#include <math.h>

uint32_t global_step_count = 0;
static MPU6050_t mpu; // La structure du driver ESEO

static uint32_t lastReadTick = 0;
static uint32_t lastStepTime = 0;
static uint8_t peakDetected = 0;

void Podometer_Init(void) {
    global_step_count = 0;

    // Initialisation du capteur
    MPU6050_Init(&mpu, NULL, 0, MPU6050_Device_0, MPU6050_Accelerometer_2G, MPU6050_Gyroscope_250s);
}

void Podometer_Update(void) {
    uint32_t now = HAL_GetTick();

    // Lecture toutes les 20 ms (50 Hz)
    if ((now - lastReadTick) >= 20) {
        lastReadTick = now;

        // Lecture des accélérations via la fonction de l'école
        MPU6050_ReadAll(&mpu);

        // Récupération des 3 axes
        float ax = (float)mpu.Accelerometer_X;
        float ay = (float)mpu.Accelerometer_Y;
        float az = (float)mpu.Accelerometer_Z;

        // Calcul de la force totale subie par le capteur
        float mag = sqrtf(ax*ax + ay*ay + az*az);

        /* --- ALGORITHME PRO : HYSTÉRÉSIS --- */
        // La gravité au repos = ~16384 (mais oscille avec le bruit)
        float seuil_haut = 22000.0f; // Force requise pour DÉTECTER le choc
        float seuil_bas  = 18000.0f; // Force requise pour VALIDER la fin du mouvement

        // 1. Détection du front montant (Le choc dépasse le bruit)
        if (mag > seuil_haut) {
            peakDetected = 1;
        }

        // 2. Validation (Le bras/la carte s'est immobilisé après le choc)
        if (peakDetected && mag < seuil_bas) {
            if (now - lastStepTime >= 300) { // Anti-rebond (1 pas max toutes les 300 ms)
                global_step_count++;
                lastStepTime = now;
            }
            peakDetected = 0; // On réarme le système pour le prochain pas
        }
    }
}

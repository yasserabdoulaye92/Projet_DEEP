/**
 * @file    podometer.c
 * @author  Yasser Abdoulaye
 * @brief   Implémentation du podomètre via MPU6050.
 *
 * @details Algorithme de détection de pas :
 *  1. Lecture des 3 axes de l'accéléromètre toutes les 20 ms (50 Hz).
 *  2. Calcul de la norme : |a| = sqrt(ax² + ay² + az²).
 *  3. Filtre passe-bas IIR (α=0.80) pour éliminer le bruit électronique.
 *  4. Soustraction de la gravité (1G) → accélération dynamique.
 *  5. Machine à états à hystérésis : le pas est compté dès le franchissement
 *     du seuil haut (front montant), le réarmement se fait sous le seuil bas.
 *
 * @section Seuils Seuils de détection
 *  - seuil_pic   = 0.15 G : franchissement -> pas compté (anti-rebond 300 ms).
 *  - seuil_repos = 0.08 G : redescente -> réarmement du détecteur.
 *
 * @section Robustesse Robustesse I2C
 *  Si le MPU6050 ne répond pas (capteur débranché, alimentation tardive),
 *  le module retente l'initialisation toutes les 2 s sans bloquer la
 *  super-boucle, au lieu d'enchaîner des timeouts I2C à chaque cycle.
 */

#include "podometer.h"
#include "stm32g4_i2c.h" /* BSP_I2C_get_handle pour le bus recovery */
#include <math.h>
#include <stdio.h>

/** @brief Compteur de pas global, incrémenté à chaque pas détecté. */
uint32_t global_step_count = 0;

/** @brief Structure interne du driver MPU6050 (I2C + données brutes). */
static MPU6050_t mpu;

/** @brief Vrai si le MPU6050 a répondu correctement à l'initialisation. */
static bool mpu_ok = false;

/** @brief Résultat du scan I2C : adresse 8 bits du 1er périphérique qui
 *         répond, ou -1 si le bus est muet. Outil de diagnostic affiché
 *         à l'écran quand le capteur est absent. */
static int16_t i2c_scan_found = -1;

/** @brief Timestamp de la dernière tentative d'init (pour le retry 2 s). */
static uint32_t derniere_tentative_init = 0;

/** @brief Timestamp du dernier cycle de lecture (ms, HAL_GetTick). */
static uint32_t lastReadTick = 0;

/** @brief Timestamp du dernier pas validé (pour l'anti-rebond). */
static uint32_t lastStepTime = 0;

/** @brief Norme d'accélération filtrée (filtre passe-bas IIR). */
static float mag_filtree = 1.0f;

/** @brief État du détecteur : 0 = armé (repos), 1 = pic en cours. */
static uint8_t etat_pas = 0;

/**
 * @brief  Débloque le bus I2C1 avant une tentative d'init (bus recovery).
 *
 * @details Procédure standard I2C de récupération d'un bus gelé :
 *  1. Désinit du périphérique (relâche les lignes, State -> RESET).
 *  2. Pins en GPIO open-drain et 9 impulsions d'horloge sur SCL :
 *     libère un esclave resté bloqué en milieu de transaction
 *     (le MPU6050 peut tenir une ligne après un branchement à chaud).
 *  3. Condition STOP manuelle (SDA 0->1 pendant SCL haut).
 *  Le BSP_I2C_Init suivant (via MPU6050_Init) reconfigure les pins en
 *  fonction alternative I2C et repart d'un périphérique propre (BUSY/ARLO
 *  nettoyés par le toggle de PE dans HAL_I2C_Init).
 */
static void I2C1_Bus_Recovery(void) {
    I2C_HandleTypeDef *h = BSP_I2C_get_handle(I2C1);
    if (h->Instance != NULL) {
        HAL_I2C_DeInit(h); /* force State=RESET -> le prochain init refait tout */
    }

    GPIO_InitTypeDef g = {0};
    g.Mode = GPIO_MODE_OUTPUT_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    g.Pin = GPIO_PIN_15; HAL_GPIO_Init(GPIOA, &g); /* SCL */
    g.Pin = GPIO_PIN_7;  HAL_GPIO_Init(GPIOB, &g); /* SDA */

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
    HAL_Delay(1);

    for (int i = 0; i < 9; i++) { /* 9 coups d'horloge pour vider l'esclave */
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);   HAL_Delay(1);
    }

    /* STOP manuel : SDA passe de 0 à 1 pendant que SCL est haut */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);  HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);   HAL_Delay(1);
}

/**
 * @brief  Tente d'initialiser le MPU6050 et mémorise le résultat.
 * @return true si le capteur répond sur le bus I2C1 (PA15/PB7).
 * @note   L'adresse I2C dépend de la broche AD0 du module (0xD0 si basse,
 *         0xD2 si haute) : on alterne les deux à chaque tentative pour
 *         fonctionner quel que soit le câblage du GY-521.
 */
static bool Podometer_TryInitMPU(void) {

    static MPU6050_Device_t addr_courante = MPU6050_Device_0;

    /* VCC du MPU6050 câblé en direct sur le rail d'alimentation (fil rouge) :
     * NULL = pas de broche GPIO d'alimentation. PA0 est réservé à l'ADC du
     * capteur de pouls — ne jamais l'utiliser ici. */
    if (MPU6050_Init(&mpu, NULL, 0, addr_courante,
                     MPU6050_Accelerometer_2G,
                     MPU6050_Gyroscope_250s) == MPU6050_Result_Ok) {
        return true;
    }

    /* Échec : on essaiera l'autre adresse à la prochaine tentative */
    addr_courante = (addr_courante == MPU6050_Device_0) ? MPU6050_Device_1
                                                        : MPU6050_Device_0;
    return false;
}

/**
 * @brief  Initialise le podomètre et démarre le MPU6050.
 * @note   Si le capteur ne répond pas, Podometer_Update() retentera
 *         l'initialisation toutes les 2 secondes automatiquement.
 */
void Podometer_Init(void) {
    global_step_count = 0;
    mag_filtree = 1.0f;
    etat_pas = 0;

    mpu_ok = Podometer_TryInitMPU();
    derniere_tentative_init = HAL_GetTick();
}

/**
 * @brief  Indique si le MPU6050 répond sur le bus I2C.
 * @retval true  Capteur détecté et initialisé.
 * @retval false Capteur absent ou muet (vérifier alim + I2C PA15/PB7).
 */
bool Podometer_IsConnected(void) {
    return mpu_ok;
}

/**
 * @brief  Résultat du dernier scan I2C (diagnostic).
 * @return Adresse 8 bits du premier périphérique détecté sur le bus,
 *         ou -1 si aucun ne répond. Le MPU6050 attendu : 0xD0 ou 0xD2.
 */
int16_t Podometer_GetI2CScan(void) {
    return i2c_scan_found;
}

/**
 * @brief  Effectue un cycle de lecture et met à jour le compteur de pas.
 * @note   Appel non bloquant : retourne immédiatement si moins de 20 ms
 *         se sont écoulées, ou si le capteur est absent (retry espacé).
 */
void Podometer_Update(void) {
    uint32_t now = HAL_GetTick();

    /* Capteur absent : on retente l'init toutes les 2 s, sans spammer
     * le bus I2C (chaque transaction sur bus mort coûte un timeout). */
    if (!mpu_ok) {
        if (now - derniere_tentative_init >= 2000) {
            derniere_tentative_init = now;

            /* 1) Déblocage du bus : 9 coups d'horloge + STOP (~25 ms) */
            I2C1_Bus_Recovery();

            /* 2) Garde-fou anti-gel : après le recovery, les pins sont en
             * GPIO relâchés — on lit l'état électrique réel des lignes.
             * Si l'une est plaquée à 0 (mauvais câblage, court-circuit),
             * la moindre transaction coûterait des timeouts de 100 ms en
             * cascade et gèlerait le menu : on ne tente RIEN tant que le
             * bus n'est pas sain. Le diagnostic SCL/SDA reste à l'écran. */
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET ||
                HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)  == GPIO_PIN_RESET) {
                i2c_scan_found = -1;
                return;
            }

            /* 3) Bus sain : tentative d'init (adresses 0xD0/0xD2 alternées) */
            mpu_ok = Podometer_TryInitMPU();

            /* 4) Échec ? Scan du bus pour afficher qui répond — rapide
             * car le bus est sain (NACK immédiat sur adresse vide). */
            if (!mpu_ok) {
                i2c_scan_found = -1;
                for (uint16_t a = 0x10; a <= 0xEE; a += 2) {
                    if (BSP_I2C_IsDeviceConnected(I2C1, (uint8_t)a)) {
                        i2c_scan_found = (int16_t)a;
                        break;
                    }
                }
            }
        }
        return;
    }

    if ((now - lastReadTick) < 20) return;
    lastReadTick = now;

    MPU6050_ReadAll(&mpu);

    /* Conversion en unités G (sensibilité ±2G : 1G = 16384 LSB) */
    float ax = (float)mpu.Accelerometer_X / 16384.0f;
    float ay = (float)mpu.Accelerometer_Y / 16384.0f;
    float az = (float)mpu.Accelerometer_Z / 16384.0f;

    float mag_brute = sqrtf(ax*ax + ay*ay + az*az);

    /* Filtre passe-bas IIR : lisse le bruit haute fréquence */
    mag_filtree = (0.80f * mag_filtree) + (0.20f * mag_brute);

    /* On retire 1G (gravité) pour isoler le mouvement dynamique */
    float choc_absolu = fabsf(mag_filtree - 1.0f);

    const float seuil_pic   = 0.15f;
    const float seuil_repos = 0.08f;

    switch (etat_pas) {
        case 0: /* Armé : le pas est compté DÈS le franchissement du seuil */
            if (choc_absolu > seuil_pic) {
                if (now - lastStepTime >= 300) { /* Anti-rebond 300 ms */
                    global_step_count++;
                    lastStepTime = now;
                }
                etat_pas = 1;
            }
            break;

        case 1: /* Pic en cours : on attend la redescente pour réarmer */
            if (choc_absolu < seuil_repos) {
                etat_pas = 0;
            }
            break;

        default:
            etat_pas = 0;
            break;
    }
}

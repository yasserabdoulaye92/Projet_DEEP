/**
 * @file hw-827.c
 * @brief Gestion de la détection des battements du cœur.
 *
 * Ce fichier contient l'algorithme utilisé pour détecter
 * les battements cardiaques à partir des valeurs du capteur.
 */

#include "hw-827.h"
/**
 * @brief Nombre de valeurs utilisées pour le filtrage.
 */
#define TAILLE_FILTRE 10
/**
 * @brief Historique des dernières mesures du capteur.
 */
static uint16_t historique[TAILLE_FILTRE];
/**
 * @brief Position actuelle dans l'historique.
 */
static uint8_t index_hist = 0;
/**
 * @brief Valeur maximale observée récemment.
 */
static uint16_t max_dynamique = 0;
/**
 * @brief Valeur minimale observée récemment.
 */
static uint16_t min_dynamique = 4095;
/**
 * @brief Indique si un pic de battement est en cours.
 */
static bool en_pic = false;

/**
 * @brief Détecte un nouveau battement cardiaque.
 *
 * Applique un filtrage sur les mesures du capteur,
 * calcule un seuil dynamique et détecte les pics
 * correspondant aux battements du cœur.
 *
 * @param adcRaw Valeur brute lue par l'ADC.
 * @param delayMsec Temps écoulé depuis la dernière mesure (ms).
 *
 * @retval true Un battement a été détecté.
 * @retval false Aucun battement détecté.
 */
bool HW827_HeartbeatDetected(uint16_t adcRaw, uint16_t delayMsec)
{
    historique[index_hist] = adcRaw;
    index_hist = (index_hist + 1) % TAILLE_FILTRE;

    uint32_t somme = 0;
    for(int i = 0; i < TAILLE_FILTRE; i++) {
        somme += historique[i];
    }
    uint16_t signal_lisse = somme / TAILLE_FILTRE;

    if (signal_lisse > max_dynamique) {
        max_dynamique = signal_lisse;
    }
    if (signal_lisse < min_dynamique) {
        min_dynamique = signal_lisse;
    }

    if (max_dynamique > 0) max_dynamique--;
    if (min_dynamique < 4095) min_dynamique++;

    uint16_t amplitude = max_dynamique - min_dynamique;

    if (amplitude < 30) {
        en_pic = false;
        return false; 
    }

    uint16_t seuil_haut = min_dynamique + (amplitude * 7) / 10;
    uint16_t seuil_bas = min_dynamique + (amplitude * 4) / 10;

    bool nouveau_battement = false;

    if (signal_lisse > seuil_haut && en_pic == false) {
        en_pic = true;
        nouveau_battement = true; 
    }
    
    else if (signal_lisse < seuil_bas) {
        en_pic = false; 
    }

    return nouveau_battement;
}

/**
 *******************************************************************************
 * @file    hw-827.c
 * @author  candice / Corrigé avec filtre anti-bruit (Moyenne glissante)
 * @brief   Implémentation de l'algorithme de détection de pouls robuste
 *******************************************************************************
 */

#include "hw-827.h"

/* --- Variables pour le filtre de lissage --- */
#define TAILLE_FILTRE 10
static uint16_t historique[TAILLE_FILTRE];
static uint8_t index_hist = 0;

/* --- Variables pour la détection dynamique --- */
static uint16_t max_dynamique = 0;
static uint16_t min_dynamique = 4095;
static bool en_pic = false;

bool HW827_HeartbeatDetected(uint16_t adcRaw, uint16_t delayMsec)
{
    // 1. FILTRAGE ANTI-BRUIT (On moyenne les 10 dernières valeurs pour lisser la ligne)
    historique[index_hist] = adcRaw;
    index_hist = (index_hist + 1) % TAILLE_FILTRE;

    uint32_t somme = 0;
    for(int i = 0; i < TAILLE_FILTRE; i++) {
        somme += historique[i];
    }
    uint16_t signal_lisse = somme / TAILLE_FILTRE;

    // 2. ADAPTATION DYNAMIQUE (Suivi de l'amplitude de la vague de sang)
    // On cherche les points les plus hauts et les plus bas de la vague
    if (signal_lisse > max_dynamique) {
        max_dynamique = signal_lisse;
    }
    if (signal_lisse < min_dynamique) {
        min_dynamique = signal_lisse;
    }

    // On referme l'étau doucement pour s'adapter si la pression du doigt change
    if (max_dynamique > 0) max_dynamique--;
    if (min_dynamique < 4095) min_dynamique++;

    // 3. MESURE DE L'AMPLITUDE
    uint16_t amplitude = max_dynamique - min_dynamique;

    // Sécurité : S'il n'y a pas assez de variation (doigt retiré ou juste du bruit électrique)
    if (amplitude < 30) {
        en_pic = false;
        return false; // Pas de battement
    }

    // 4. DÉTECTION DU BATTEMENT
    // On déclenche quand le signal monte au-dessus de 70% de la hauteur de la vague
    uint16_t seuil_haut = min_dynamique + (amplitude * 7) / 10;
    // On réarme quand le signal redescend en dessous de 40%
    uint16_t seuil_bas = min_dynamique + (amplitude * 4) / 10;

    bool nouveau_battement = false;

    // Front montant (La vague de sang arrive)
    if (signal_lisse > seuil_haut && en_pic == false) {
        en_pic = true;
        nouveau_battement = true; // BINGO ! Un battement valide.
    }
    // Front descendant (Le sang repart)
    else if (signal_lisse < seuil_bas) {
        en_pic = false; // On est dans le creux, prêt pour le prochain battement
    }

    return nouveau_battement;
}

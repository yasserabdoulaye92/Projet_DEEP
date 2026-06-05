/**
 * @file    menu.c
 * @brief   Implémentation de l'interface graphique et de la navigation tactile.
 */

#include "menu.h"
#include <string.h>
#include <stdio.h>

extern uint32_t global_step_count;          /**< Nombre de pas global (podomètre) */
extern void Backlight_Set(uint8_t pourcentage); /**< Réglage du rétroéclairage */
extern volatile uint32_t dernier_appui_tactile;  /**< Timestamp du dernier appui tactile */

#define ILI9341_COLOR_VIOLET 0xF81F

static Page_t pageActuelle = PAGE_MENU;     /**< Page actuellement affichée */
static char nom_selection[15] = "";         /**< Nom de l’application sélectionnée */
static uint8_t mode_sante = 0;              /**< Mode santé: 0=podomètre, 1=cardio */

static char text_heure[15] = "12:00:00";    /**< Heure affichée */
static char text_notif[50] = "Aucun message"; /**< Texte notification */
static char meteo_temp[15] = "--";          /**< Données météo */

static uint16_t theme_color = ILI9341_COLOR_GREEN; /**< Couleur du thème */
static uint8_t chrono_actif = 0;            /**< Chronomètre actif */
static uint32_t chrono_secondes = 0;        /**< Temps du chronomètre */
static uint8_t luminosite = 100;            /**< Luminosité écran */

#define NB_APPS 6

/**
 * @brief Structure représentant un bouton d'application.
 */
typedef struct {
    uint16_t x;            /**< Position X */
    uint16_t y;            /**< Position Y */
    char nom[15];          /**< Nom affiché */
    Page_t page_cible;     /**< Page associée */
    uint8_t icon_id;       /**< ID icône */
} AppButton_t;

/**
 * @brief Liste des applications du menu principal.
 */
static AppButton_t apps[NB_APPS] = {
    {25, 35, "Horloge", PAGE_HORLOGE, 0},
    {135, 35, "Notifs", PAGE_NOTIF, 1},
    {245, 35, "Sante", PAGE_SANTE, 2},
    {25, 135, "Meteo", PAGE_METEO, 3},
    {135, 135, "NFC", PAGE_NFC, 4},
    {245, 135, "Options", PAGE_REGLAGES, 5}
};

/**
 * @brief Dessine un cadre rectangulaire.
 */
static void Ma_Boite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Efface une zone de l'écran.
 */
static void Effacer_Zone(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * @brief Dessine une icône d’application.
 */
static void Dessiner_Icone(uint16_t x, uint16_t y, char* nom, uint8_t id);

/**
 * @brief Dessine une icône de cœur.
 */
static void draw_heart_icon(int16_t x, int16_t y, int16_t size, uint16_t color);

/**
 * @brief Initialise le menu et l’écran.
 */
void MENU_init(void);

/**
 * @brief Met à jour l’heure affichée.
 */
void MENU_update_time(char* time_str);

/**
 * @brief Met à jour le message de notification.
 */
void MENU_set_notif(char* texte);

/**
 * @brief Met à jour les données météo.
 */
void MENU_set_meteo(char* infos);

/**
 * @brief Met à jour l’affichage du capteur santé.
 */
void MENU_update_sante(uint16_t adc_val, int bpm, bool is_beating);

/**
 * @brief Dessine complètement la page actuelle.
 */
void MENU_draw(void);

/**
 * @brief Gère les interactions tactiles et la navigation.
 */
void MENU_handler(void);

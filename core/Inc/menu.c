#include "menu.h"
<<<<<<< Updated upstream
#include <stdio.h>

// Définition des différentes vues possibles

// Variable qui mémorise la page où l'on se trouve
static Page_t pageActuelle = PAGE_PRINCIPALE;

// Noms pour le menu principal
static const char* noms_cases[5] = {"Time", "Graph", "Calc", "Map", "Music"};

// Couleurs style "Dark Mode" Smartphone
#define COLOR_APP_BG      0x2104  // Gris anthracite
#define COLOR_APP_BORDER  0x4208  // Gris moyen
#define COLOR_APP_SELECT  ILI9341_COLOR_BLUE

void drawbutton(void){
 //Bouton en bas de l'écran (Adapté à la rotation paysage)
  uint16_t x = 70, y = 200, w = 100, h = 35;
  ILI9341_DrawFilledRectangle(x, y, w, h, ILI9341_COLOR_BLACK);
  ILI9341_DrawRectangle(x, y, w, h, ILI9341_COLOR_WHITE);
  ILI9341_Puts(x + 15, y + 10, "RETOUR", &Font_11x18, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
}


void MENU_init(void) {
    ILI9341_Init();
    XPT2046_init();

    ILI9341_Fill(ILI9341_COLOR_BLACK);
    HAL_Delay(100);

=======
#include <string.h>

// Variables globales
static Page_t pageActuelle = PAGE_MENU;
static char nom_selection[15] = "";
static char text_notif[50] = "Aucun message"; // Mémoire pour la notification

#define NB_APPS 6
#define THEME_COLOR ILI9341_COLOR_GREEN

typedef struct {
    uint16_t x;
    uint16_t y;
    char nom[15];
    Page_t page_cible;
    uint8_t icon_id;
} AppButton_t;

static AppButton_t apps[NB_APPS] = {
    {25,  35, "Horloge", PAGE_HORLOGE,  0},
    {135, 35, "Notifs",  PAGE_NOTIF,    1},
    {245, 35, "Sante",   PAGE_SANTE,    2},
    {25,  135, "Meteo",   PAGE_METEO,    3},
    {135, 135, "NFC",     PAGE_NFC,      4},
    {245, 135, "Options", PAGE_REGLAGES, 5}
};

// ==========================================
// NOUVEAU : RÉCEPTION DE LA NOTIFICATION
// ==========================================
void MENU_set_notif(char* texte) {
    // On copie le texte reçu dans notre mémoire (max 49 caractères)
    strncpy(text_notif, texte, 49);
    text_notif[49] = '\0'; // Sécurité pour fermer la chaîne

    // Si on est DÉJÀ en train de regarder la page Notifs, on rafraîchit l'écran !
    if (pageActuelle == PAGE_NOTIF) {
        MENU_draw();
    }
}

// ==========================================
// FONCTION BLINDÉE POUR REMPLACER LE RECTANGLE
// ==========================================
void Ma_Boite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    ILI9341_DrawLine(x, y, x + w, y, color);
    ILI9341_DrawLine(x, y + h, x + w, y + h, color);
    ILI9341_DrawLine(x, y, x, y + h, color);
    ILI9341_DrawLine(x + w, y, x + w, y + h, color);
}

// ==========================================
// DESSIN DES ICÔNES
// ==========================================
void Dessiner_Icone(uint16_t x, uint16_t y, char* nom, uint8_t id) {
    Ma_Boite(x, y, 50, 50, THEME_COLOR);
    Ma_Boite(x + 2, y + 2, 46, 46, THEME_COLOR);

    uint16_t cx = x + 25;
    uint16_t cy = y + 25;

    switch(id) {
        case 0: // Horloge
            ILI9341_DrawCircle(cx, cy, 14, THEME_COLOR);
            ILI9341_DrawLine(cx, cy, cx, cy - 8, THEME_COLOR);
            ILI9341_DrawLine(cx, cy, cx + 6, cy + 3, THEME_COLOR);
            break;
        case 1: // Notifs
            Ma_Boite(cx - 8, cy - 12, 16, 24, THEME_COLOR);
            ILI9341_DrawCircle(cx, cy + 8, 2, THEME_COLOR);
            ILI9341_DrawLine(cx - 4, cy - 8, cx + 4, cy - 8, THEME_COLOR);
            break;
        case 2: // Santé
            ILI9341_DrawLine(cx, cy - 10, cx, cy + 10, THEME_COLOR);
            ILI9341_DrawLine(cx - 1, cy - 10, cx - 1, cy + 10, THEME_COLOR);
            ILI9341_DrawLine(cx + 1, cy - 10, cx + 1, cy + 10, THEME_COLOR);
            ILI9341_DrawLine(cx - 10, cy, cx + 10, cy, THEME_COLOR);
            ILI9341_DrawLine(cx - 10, cy - 1, cx + 10, cy - 1, THEME_COLOR);
            ILI9341_DrawLine(cx - 10, cy + 1, cx + 10, cy + 1, THEME_COLOR);
            break;
        case 3: // Météo
            ILI9341_DrawCircle(cx, cy, 6, THEME_COLOR);
            ILI9341_DrawLine(cx, cy - 9, cx, cy - 14, THEME_COLOR);
            ILI9341_DrawLine(cx, cy + 9, cx, cy + 14, THEME_COLOR);
            ILI9341_DrawLine(cx - 9, cy, cx - 14, cy, THEME_COLOR);
            ILI9341_DrawLine(cx + 9, cy, cx + 14, cy, THEME_COLOR);
            break;
        case 4: // NFC
            ILI9341_DrawCircle(cx - 6, cy + 6, 2, THEME_COLOR);
            ILI9341_DrawCircle(cx, cy, 6, THEME_COLOR);
            ILI9341_DrawCircle(cx + 4, cy - 4, 12, THEME_COLOR);
            break;
        case 5: // Options
            ILI9341_DrawCircle(cx, cy, 8, THEME_COLOR);
            ILI9341_DrawCircle(cx, cy, 3, THEME_COLOR);
            ILI9341_DrawLine(cx, cy - 12, cx, cy + 12, THEME_COLOR);
            ILI9341_DrawLine(cx - 12, cy, cx + 12, cy, THEME_COLOR);
            break;
    }

    ILI9341_Puts(x + 2, y + 55, nom, &Font_7x10, THEME_COLOR, ILI9341_COLOR_BLACK);
}

void MENU_init(void) {
    #if USE_XPT2046
        XPT2046_init();
        HAL_Delay(50);
    #endif

    ILI9341_Init();
    HAL_Delay(50);
>>>>>>> Stashed changes
    MENU_draw();
}

void MENU_draw(void) {
    ILI9341_Fill(ILI9341_COLOR_BLACK);

<<<<<<< Updated upstream
    if (pageActuelle == PAGE_PRINCIPALE) {
        // --- TITRE "Menu"
        ILI9341_Puts(90, 15, "Menu", &Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);

        // --- DESSIN DES 5 ICONES (Grille 2x3 centrée) ---
        // On définit la couleur des icônes (Jaune/Vert)
        uint16_t iconColor = ILI9341_COLOR_YELLOW;

        // Les noms des applications correspondants aux icônes
        const char* iconNames[5] = {"Time", "Graph", "Calc", "Map", "Music"};
        // Caractères "icônes" simulés (o=horloge, g=graphe, +=calc, m=carte, ♫=musique)
        const char iconSymbols[5] = {'o', 'g', '+', 'm', '4'};

        for (int i = 0; i < 5; i++) {
            // Calcul des positions pour centrer (2 colonnes, 3 lignes)
            // Colonne 0 : x=60 | Colonne 1 : x=140
            uint16_t x = 60 + ((i % 2) * 80);
            // Ligne 0 : y=50 | Ligne 1 : y=110 | Ligne 2 : y=170
            uint16_t y = 50 + ((i / 2) * 60);

            // A. Le contour de l'icône (Carré Jaune)
            ILI9341_DrawRectangle(x, y, 50, 50, iconColor);

            // B. Le symbole de l'icône (plus gros, centré dans le carré)
            // On utilise Font_16x26 pour que ça ressemble à un logo
            char sym[2] = {iconSymbols[i], '\0'};
            ILI9341_Puts(x + 15, y + 12, sym, &Font_16x26, iconColor, ILI9341_COLOR_BLACK);

            // C. Le nom de l'application en petit en dessous (Vert)
            ILI9341_Puts(x + 5, y + 55, (char*)iconNames[i], &Font_7x10, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
        }
    }
    else {
        // --- SOUS-MENU -------
        ILI9341_Puts(70, 20, (char*)noms_cases[pageActuelle - 1], &Font_11x18, ILI9341_COLOR_CYAN, ILI9341_COLOR_BLACK);
        ILI9341_Puts(40, 100, "CONTENU ACTIF", &Font_11x18, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);

        // BOUTON RETOUR EN BAS A DROITE
        uint16_t bX = 135, bY = 250, bW = 90, bH = 45;
        ILI9341_DrawFilledRectangle(bX, bY, bW, bH, ILI9341_COLOR_GRAY); // Button gris
        ILI9341_Puts(145, 265, "RETOUR", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_GRAY); // Texte en noir
    }
}
void MENU_handler(void) {
    int16_t x, y;

    // Si on touche l'écran
    if (XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE)) {

        if (pageActuelle == PAGE_PRINCIPALE) {
            // Logique de détection pour les 5 ICONES (Grille 2x3 centrée)
            for (int i = 0; i < 5; i++) {
                // Calcul des MÊMES coordonnées que dans MENU_draw
                uint16_t x_icon = 60 + ((i % 2) * 80);
                uint16_t y_icon = 50 + ((i / 2) * 60);

                // On vérifie si ton doigt est dans le carré de 50x50 pixels
                if (x > x_icon && x < (x_icon + 50) && y > y_icon && y < (y_icon + 50)) {
                    // On change de page (i+1 car PAGE_PRINCIPALE = 0)
                    pageActuelle = (Page_t)(i + 1);
                    MENU_draw();    // On redessine le sous-menu
                    HAL_Delay(500); // Anti-rebond généreux
                    return;
                }
            }
        }
        else {
            // --- LOGIQUE POUR SORTIR  ---
            // Zone : x entre 135 et 225, y entre 250 et 295
            if (x > 135 && x < 225 && y > 250 && y < 295) {
                pageActuelle = PAGE_PRINCIPALE; // Retour au menu
                MENU_draw();                    // Redessine la grille d'icônes
                HAL_Delay(500);                 // Anti-rebond
            }
        }
    }
=======
    if (pageActuelle == PAGE_MENU) {
        ILI9341_Puts(135, 5, "12:34", &Font_7x10, THEME_COLOR, ILI9341_COLOR_BLACK);
        ILI9341_DrawLine(10, 20, 310, 20, THEME_COLOR);

        for (int i = 0; i < NB_APPS; i++) {
            Dessiner_Icone(apps[i].x, apps[i].y, apps[i].nom, apps[i].icon_id);
        }
    }
    else {
        // --- PAGE D'APPLICATION ---
        ILI9341_Puts(20, 20, nom_selection, &Font_16x26, THEME_COLOR, ILI9341_COLOR_BLACK);
        ILI9341_DrawLine(20, 50, 300, 50, THEME_COLOR);

        // AFFICHAGE SPÉCIFIQUE SELON LA PAGE
        if (pageActuelle == PAGE_NOTIF) {
            ILI9341_Puts(20, 70, "Nouveau message :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
            // On affiche le texte reçu du téléphone en gros (Police 11x18)
            ILI9341_Puts(20, 100, text_notif, &Font_11x18, THEME_COLOR, ILI9341_COLOR_BLACK);
        } else {
            ILI9341_Puts(20, 80, "> SYS ACTIVE", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
        }

        // --- BOUTON RETOUR ANTICHAR ---
        Ma_Boite(85, 180, 150, 45, THEME_COLOR);
        Ma_Boite(87, 182, 146, 41, THEME_COLOR);
        ILI9341_Puts(105, 190, "RETOUR", &Font_16x26, THEME_COLOR, ILI9341_COLOR_BLACK);
    }
}

void MENU_handler(void) {
    #if USE_XPT2046
        int16_t x, y;

        if (XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE)) {

            if (pageActuelle == PAGE_MENU) {
                for (int i = 0; i < NB_APPS; i++) {
                    if (x >= apps[i].x && x <= (apps[i].x + 50) &&
                        y >= apps[i].y && y <= (apps[i].y + 70))
                    {
                        strcpy(nom_selection, apps[i].nom);
                        pageActuelle = apps[i].page_cible;
                        MENU_draw();
                        HAL_Delay(300);
                        return;
                    }
                }
            } else {
                if (x >= 85 && x <= 235 && y >= 180 && y <= 225) {
                    pageActuelle = PAGE_MENU;
                    MENU_draw();
                    HAL_Delay(300);
                }
            }
        }
    #endif
>>>>>>> Stashed changes
}

#include "menu.h"
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

    MENU_draw();
}

void MENU_draw(void) {
    ILI9341_Fill(ILI9341_COLOR_BLACK);

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
}

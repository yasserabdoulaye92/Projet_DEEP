#include "menu.h"
#include <stdio.h>

// Définition des différentes vues possibles

// Variable qui mémorise la page où l'on se trouve
static Page_t pageActuelle = PAGE_PRINCIPALE;

// Noms pour le menu principal
static const char* noms_cases[5] = {"Santé", "Horloge", "Cartes", "Games", "Méteo",};

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

// Tableau pour gérer les noms des cases facilement

void MENU_init(void) {
    ILI9341_Init();
    XPT2046_init();
    MENU_draw();
}

void MENU_draw(void) {
    // 1. On force le noir total
    ILI9341_Fill(0x0000);
    HAL_Delay(30); // On laisse le temps à l'écran de réagir

    if (pageActuelle == PAGE_PRINCIPALE) {
        // TITRE "Menu" en vert (comme sur ta photo)
        ILI9341_Puts(90, 15, "Menu", &Font_11x18, ILI9341_COLOR_GREEN, 0x0000);

        // --- DESSIN DES 2 PREMIERES ICONES POUR TESTER ---
        // Icône 1 : Horloge (Jaune)
        ILI9341_DrawRectangle(60, 60, 50, 50, ILI9341_COLOR_YELLOW);
        ILI9341_Puts(78, 75, "H", &Font_11x18, ILI9341_COLOR_YELLOW, 0x0000);
        ILI9341_Puts(65, 115, "Time", &Font_7x10, ILI9341_COLOR_GREEN, 0x0000);

        // Icône 2 : Games (Jaune)
        ILI9341_DrawRectangle(140, 60, 50, 50, ILI9341_COLOR_YELLOW);
        ILI9341_Puts(158, 75, "G", &Font_11x18, ILI9341_COLOR_YELLOW, 0x0000);
        ILI9341_Puts(145, 115, "Game", &Font_7x10, ILI9341_COLOR_GREEN, 0x0000);
    }
    else {
        // --- SOUS-MENU ---
        ILI9341_Puts(60, 20, "CONTENU", &Font_11x18, ILI9341_COLOR_WHITE, 0x0000);

        // --- BOUTON RETOUR EN BAS A DROITE ---
        // On le dessine en GRIS (0x7BEF) pour qu'il soit visible sur le noir
        ILI9341_DrawFilledRectangle(140, 240, 80, 40, 0x7BEF);
        ILI9341_Puts(150, 255, "RETOUR", &Font_7x10, 0x0000, 0x7BEF);
    }
}
void MENU_handler(void) {
    int16_t x, y;
    if (XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE)) {
        if (pageActuelle == PAGE_PRINCIPALE) {
            // Détection icône Time (60, 60)
            if (x > 60 && x < 110 && y > 60 && y < 110) {
                pageActuelle = PAGE_HORLOGE;
                MENU_draw();
                HAL_Delay(500);
            }
            // Détection icône Games (140, 60)
            else if (x > 140 && x < 190 && y > 60 && y < 110) {
                pageActuelle = PAGE_GAMES;
                MENU_draw();
                HAL_Delay(500);
            }
        }
        else {
            // BOUTON RETOUR (bas-droite : 140, 240)
            if (x > 140 && x < 220 && y > 240 && y < 280) {
                pageActuelle = PAGE_PRINCIPALE;
                MENU_draw();
                HAL_Delay(500);
            }
        }
    }
}

#include "config.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>

void SystemClock_Config(void);
void DrawGrid(void);
void DetectTouch(void);

// Variables globales pour le tactile
int16_t x_t, y_t;

// Paramètres de la grille
#define ROWS 5
#define COLS 2
#define CELL_W 100
#define CELL_H 50
#define SPACING 15

void DrawGrid(void) {
    ILI9341_Fill(ILI9341_COLOR_WHITE); // Fond blanc
    ILI9341_Puts(60, 5, "TEST 10 CASES", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            uint16_t x = 15 + (j * (CELL_W + SPACING));
            uint16_t y = 35 + (i * (CELL_H + SPACING));
            // Dessine le contour de la case
            ILI9341_DrawRectangle(x, y, CELL_W, CELL_H, ILI9341_COLOR_BLACK);

            // Affiche le numéro de la case
            char num[3];
            sprintf(num, "%d", (i * COLS) + j + 1);
            ILI9341_Puts(x + 40, y + 15, num, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        }
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();

    ILI9341_Init();
    XPT2046_init();

    DrawGrid();

    int16_t x_t, y_t; // Variables pour stocker le toucher

    while (1) {
        // Lecture du tactile avec la fonction de ton fichier .h
        if (XPT2046_getCoordinates(&x_t, &y_t, XPT2046_COORDINATE_SCREEN_RELATIVE)) {

            // Parcourir les 10 zones pour voir laquelle est touchée
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    uint16_t x_pos = 15 + (j * (CELL_W + SPACING));
                    uint16_t y_pos = 35 + (i * (CELL_H + SPACING));

                    // Vérification si le doigt est dans cette case
                    if (x_t > x_pos && x_t < (x_pos + CELL_W) && y_t > y_pos && y_t < (y_pos + CELL_H)) {

                        // Action : La case devient BLEUE quand on appuie
                        ILI9341_DrawRectangle(x_pos, y_pos, CELL_W, CELL_H, ILI9341_COLOR_BLUE);
                        ILI9341_Puts(10, 300, "Case Touchee !      ", &Font_11x18, ILI9341_COLOR_BLUE, ILI9341_COLOR_WHITE);

                        HAL_Delay(200); // Petit délai pour voir la couleur

                        // Remettre la case en NOIR après l'appui
                        ILI9341_DrawRectangle(x_pos, y_pos, CELL_W, CELL_H, ILI9341_COLOR_BLACK);
                    }
                }
            }
        }
    }
}

/**
 ******************************************************************************
 * @file    menu.c
 * @brief   Gestion du menu principal (affichage + tactile)
 ******************************************************************************
 */

#include "menu.h"
#include "config.h"
#include "tft_ili9341/ILI9341.h"
#include "stm32g4_xpt2046.h"
#include "stm32g4xx_hal.h"
#include <string.h>

/* ==================================================================
   Items du menu
   ================================================================== */
static const char *menuLabels[MENU_ITEM_COUNT] = {
    " Podometre",
    " Reinitialiser"
};

/* ==================================================================
   Variable privée : flag reset
   ================================================================== */
static uint8_t resetRequested = 0;

/* ==================================================================
   Dessin d'un seul item
   ================================================================== */
static void DrawItem(uint8_t index, uint8_t selected)
{
    uint16_t y      = MENU_START_Y + index * MENU_ITEM_H;
    uint16_t bgCol  = selected ? COLOR_MENU_SEL  : COLOR_BG;
    uint16_t txtCol = selected ? ILI9341_WHITE    : COLOR_MENU_NORM;

    /* Fond */
    ILI9341_FillRect(10, y, SCREEN_WIDTH - 20, MENU_ITEM_H - 4, bgCol);

    /* Bordure */
    ILI9341_DrawRect(10, y, SCREEN_WIDTH - 20, MENU_ITEM_H - 4, COLOR_ACCENT);

    /* Texte */
    ILI9341_WriteString(20, y + 12,
                        menuLabels[index],
                        Font_11x18,
                        txtCol, bgCol);
}

/* ==================================================================
   Menu_Init : efface l'écran et dessine le menu complet
   ================================================================== */
void Menu_Init(void)
{
    ILI9341_FillScreen(COLOR_BG);

    /* Titre */
    ILI9341_WriteString(70, 15,
                        "MENU PRINCIPAL",
                        Font_11x18,
                        COLOR_TITLE, COLOR_BG);

    /* Ligne de séparation */
    ILI9341_DrawHLine(10, 50, SCREEN_WIDTH - 20, COLOR_ACCENT);

    /* Items */
    for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++)
        DrawItem(i, 0);
}

/* ==================================================================
   Menu_HandleTouch : lit le tactile et retourne le nouvel AppState
   ================================================================== */
AppState Menu_HandleTouch(void)
{
    uint16_t tx, ty;

    if (!XPT2046_GetTouch(&tx, &ty))
        return APP_STATE_MENU;  /* Pas de toucher */

    for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++)
    {
        uint16_t y0 = MENU_START_Y + i * MENU_ITEM_H;
        uint16_t y1 = y0 + MENU_ITEM_H - 4;

        if (ty >= y0 && ty <= y1 && tx >= 10 && tx <= (SCREEN_WIDTH - 10))
        {
            /* Retour visuel */
            DrawItem(i, 1);
            HAL_Delay(150);
            DrawItem(i, 0);

            if (i == 0)
            {
                /* → Podomètre */
                return APP_STATE_PODOMETER;
            }
            else if (i == 1)
            {
                /* → Réinitialiser : on lève le flag et on reste dans le menu */
                resetRequested = 1;
                return APP_STATE_MENU;
            }
        }
    }

    return APP_STATE_MENU;
}

/* ==================================================================
   Menu_IsResetRequested : lecture + auto-clear du flag reset
   ================================================================== */
uint8_t Menu_IsResetRequested(void)
{
    uint8_t r    = resetRequested;
    resetRequested = 0;   /* auto-clear : ne se déclenche qu'une fois */
    return r;
}

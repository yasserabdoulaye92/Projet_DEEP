/**
 * @file    menu.c
 * @brief   Machine à états de l'affichage et traitement graphique des applications.
 */

#include "menu.h"
#include <string.h>
#include <stdio.h>

static Page_t pageActuelle = PAGE_MENU;
static char nom_selection[15] = "";

static char text_heure[15] = "--:--:--";
static char text_notif[50] = "Aucun message";
static char meteo_etat[15] = "Inconnu";
static char meteo_temp[10] = "--";

static uint16_t theme_color = ILI9341_COLOR_GREEN;

static uint8_t chrono_actif = 0;
static uint32_t chrono_secondes = 0;

#define NB_APPS 6

typedef struct {
    uint16_t x;
    uint16_t y;
    char nom[15];
    Page_t page_cible;
    uint8_t icon_id;
} AppButton_t;

static AppButton_t apps[NB_APPS] = {
    {25,  35,  "Horloge", PAGE_HORLOGE,  0},
    {135, 35,  "Notifs",  PAGE_NOTIF,    1},
    {245, 35,  "Sante",   PAGE_SANTE,    2},
    {25,  135, "Meteo",   PAGE_METEO,    3},
    {135, 135, "NFC",     PAGE_NFC,      4},
    {245, 135, "Options", PAGE_REGLAGES, 5}
};

static void Ma_Boite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    ILI9341_DrawLine(x, y, x + w, y, color);
    ILI9341_DrawLine(x, y + h, x + w, y + h, color);
    ILI9341_DrawLine(x, y, x, y + h, color);
    ILI9341_DrawLine(x + w, y, x + w, y + h, color);
}

static void Dessiner_Icone(uint16_t x, uint16_t y, char* nom, uint8_t id) {
    Ma_Boite(x, y, 50, 50, theme_color);
    Ma_Boite(x + 2, y + 2, 46, 46, theme_color);
    uint16_t cx = x + 25; uint16_t cy = y + 25;

    switch(id) {
        case 0:
            ILI9341_DrawCircle(cx, cy, 14, theme_color);
            ILI9341_DrawLine(cx, cy, cx, cy - 8, theme_color);
            ILI9341_DrawLine(cx, cy, cx + 6, cy + 3, theme_color);
            break;
        case 1:
            Ma_Boite(cx - 8, cy - 12, 16, 24, theme_color);
            ILI9341_DrawCircle(cx, cy + 8, 2, theme_color);
            ILI9341_DrawLine(cx - 4, cy - 8, cx + 4, cy - 8, theme_color);
            break;
        case 2:
            ILI9341_DrawLine(cx, cy - 10, cx, cy + 10, theme_color);
            ILI9341_DrawLine(cx - 1, cy - 10, cx - 1, cy + 10, theme_color);
            ILI9341_DrawLine(cx + 1, cy - 10, cx + 1, cy + 10, theme_color);
            ILI9341_DrawLine(cx - 10, cy, cx + 10, cy, theme_color);
            ILI9341_DrawLine(cx - 10, cy - 1, cx + 10, cy - 1, theme_color);
            ILI9341_DrawLine(cx - 10, cy + 1, cx + 10, cy + 1, theme_color);
            break;
        case 3:
            ILI9341_DrawCircle(cx, cy, 6, theme_color);
            ILI9341_DrawLine(cx, cy - 9, cx, cy - 14, theme_color);
            ILI9341_DrawLine(cx, cy + 9, cx, cy + 14, theme_color);
            ILI9341_DrawLine(cx - 9, cy, cx - 14, cy, theme_color);
            ILI9341_DrawLine(cx + 9, cy, cx + 14, cy, theme_color);
            break;
        case 4:
            ILI9341_DrawCircle(cx - 6, cy + 6, 2, theme_color);
            ILI9341_DrawCircle(cx, cy, 6, theme_color);
            ILI9341_DrawCircle(cx + 4, cy - 4, 12, theme_color);
            break;
        case 5:
            ILI9341_DrawCircle(cx, cy, 8, theme_color);
            ILI9341_DrawCircle(cx, cy, 3, theme_color);
            ILI9341_DrawLine(cx, cy - 12, cx, cy + 12, theme_color);
            ILI9341_DrawLine(cx - 12, cy, cx + 12, cy, theme_color);
            break;
    }
    ILI9341_Puts(x + 2, y + 55, nom, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
}

void MENU_init(void) {
    #if USE_XPT2046
        XPT2046_init();
        HAL_Delay(50);
    #endif
    ILI9341_Init();
    HAL_Delay(50);
    MENU_draw();
}

void MENU_update_time(char* time_str) {
    strcpy(text_heure, time_str);

    if (pageActuelle == PAGE_MENU) {
        ILI9341_Puts(125, 5, text_heure, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
    }

    if (chrono_actif) {
        chrono_secondes++;
    }

    if (pageActuelle == PAGE_HORLOGE) {
        ILI9341_Puts(80, 55, text_heure, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
        char chrono_str[15];
        sprintf(chrono_str, "%02d:%02d", (int)(chrono_secondes / 60), (int)(chrono_secondes % 60));
        ILI9341_Puts(115, 125, chrono_str, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
    }
}

void MENU_set_notif(char* texte) {
    strncpy(text_notif, texte, 49);
    text_notif[49] = '\0';
    if (pageActuelle == PAGE_NOTIF) { MENU_draw(); }
}

void MENU_set_meteo(char* etat, char* temp) {
    strncpy(meteo_etat, etat, 14);
    meteo_etat[14] = '\0';
    strncpy(meteo_temp, temp, 9);
    meteo_temp[9] = '\0';
    if (pageActuelle == PAGE_METEO) { MENU_draw(); }
}

void MENU_draw(void) {
    ILI9341_Fill(ILI9341_COLOR_BLACK);

    if (pageActuelle == PAGE_MENU) {
        ILI9341_Puts(125, 5, text_heure, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
        ILI9341_DrawLine(10, 20, 310, 20, theme_color);

        for (int i = 0; i < NB_APPS; i++) {
            Dessiner_Icone(apps[i].x, apps[i].y, apps[i].nom, apps[i].icon_id);
        }
    }
    else {
        ILI9341_Puts(20, 10, nom_selection, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
        ILI9341_DrawLine(20, 40, 300, 40, theme_color);

        switch(pageActuelle) {
            case PAGE_HORLOGE:
                ILI9341_Puts(20, 45, "Heure RTC :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                ILI9341_Puts(80, 55, text_heure, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);

                ILI9341_Puts(20, 110, "Chronomethre :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                Ma_Boite(20, 120, 80, 30, theme_color);
                ILI9341_Puts(28, 130, chrono_actif ? "STOP" : "START", &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
                Ma_Boite(220, 120, 80, 30, theme_color);
                ILI9341_Puts(242, 130, "RESET", &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
                break;

            case PAGE_NOTIF:
                ILI9341_Puts(20, 55, "Dernier SMS / Message :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                ILI9341_Puts(20, 85, text_notif, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
                break;

            case PAGE_SANTE:
                ILI9341_Puts(20, 60, "Espace Sante", &Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                ILI9341_Puts(20, 100, "[ à faire Candice ]", &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
                break;

            case PAGE_METEO:
                ILI9341_Puts(20, 50, "Meteo locale :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                char temp_str[15];
                sprintf(temp_str, "%s C", meteo_temp);
                ILI9341_Puts(20, 70, temp_str, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
                ILI9341_Puts(20, 105, meteo_etat, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);

                if (strncmp(meteo_etat, "Soleil", 6) == 0) {
                    ILI9341_DrawCircle(220, 85, 20, ILI9341_COLOR_YELLOW);
                    for(int a=0; a<360; a+=45) ILI9341_DrawLine(220, 85, 220, 55, ILI9341_COLOR_YELLOW);
                }
                else if (strncmp(meteo_etat, "Nuage", 5) == 0) {
                    ILI9341_DrawCircle(210, 85, 10, ILI9341_COLOR_CYAN);
                    ILI9341_DrawCircle(230, 85, 15, ILI9341_COLOR_CYAN);
                    ILI9341_DrawCircle(250, 90, 10, ILI9341_COLOR_CYAN);
                }
                else if (strncmp(meteo_etat, "Pluie", 5) == 0) {
                    ILI9341_DrawLine(210, 80, 200, 100, ILI9341_COLOR_CYAN);
                    ILI9341_DrawLine(230, 80, 220, 100, ILI9341_COLOR_CYAN);
                    ILI9341_DrawLine(250, 80, 240, 100, ILI9341_COLOR_CYAN);
                }
                break;

            case PAGE_NFC:
                ILI9341_Puts(20, 60, "Lecteur NFC", &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
                ILI9341_Puts(20, 100, "Statut : Pret a scanner...", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                break;

            case PAGE_REGLAGES:
                ILI9341_Puts(20, 50, "Choisir la couleur globale :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                Ma_Boite(20, 80, 70, 40, ILI9341_COLOR_GREEN);
                ILI9341_Puts(35, 95, "VERT", &Font_7x10, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
                Ma_Boite(125, 80, 70, 40, ILI9341_COLOR_CYAN);
                ILI9341_Puts(140, 95, "BLEU", &Font_7x10, ILI9341_COLOR_CYAN, ILI9341_COLOR_BLACK);
                Ma_Boite(230, 80, 70, 40, ILI9341_COLOR_YELLOW);
                ILI9341_Puts(245, 95, "JAUNE", &Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
                break;
        }

        Ma_Boite(85, 180, 150, 45, theme_color);
        Ma_Boite(87, 182, 146, 41, theme_color);
        ILI9341_Puts(110, 190, "RETOUR", &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
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
            }
            else {
                if (x >= 85 && x <= 235 && y >= 180 && y <= 225) {
                    pageActuelle = PAGE_MENU;
                    MENU_draw();
                    HAL_Delay(300);
                    return;
                }

                if (pageActuelle == PAGE_HORLOGE) {
                    if (x >= 20 && x <= 100 && y >= 120 && y <= 150) {
                        chrono_actif = !chrono_actif;
                        MENU_draw();
                        HAL_Delay(300);
                    }
                    if (x >= 220 && x <= 300 && y >= 120 && y <= 150) {
                        chrono_actif = 0;
                        chrono_secondes = 0;
                        MENU_draw();
                        HAL_Delay(300);
                    }
                }

                if (pageActuelle == PAGE_REGLAGES) {
                    if (x >= 20 && x <= 90 && y >= 80 && y <= 120) {
                        theme_color = ILI9341_COLOR_GREEN;
                        MENU_draw();
                        HAL_Delay(300);
                    }
                    if (x >= 125 && x <= 195 && y >= 80 && y <= 120) {
                        theme_color = ILI9341_COLOR_CYAN;
                        MENU_draw();
                        HAL_Delay(300);
                    }
                    if (x >= 230 && x <= 300 && y >= 80 && y <= 120) {
                        theme_color = ILI9341_COLOR_YELLOW;
                        MENU_draw();
                        HAL_Delay(300);
                    }
                }
            }
        }
    #endif
}

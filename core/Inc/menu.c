/**
 * @file    menu.c
 * @brief   Implémentation de l'interface graphique et de la navigation tactile.
 */

#include "menu.h"
#include <string.h>
#include <stdio.h>

/* --- Variables externes --- */
extern uint32_t global_step_count;
extern void Backlight_Set(uint8_t pourcentage);
extern volatile uint32_t dernier_appui_tactile;

#define ILI9341_COLOR_VIOLET 0xF81F

/* --- Variables d'état --- */
static Page_t pageActuelle = PAGE_MENU;
static char nom_selection[15] = "";
static uint8_t mode_sante = 0; // 0 = Podomètre, 1 = Cardio

static char text_heure[15] = "12:00:00";
static char text_notif[50] = "Aucun message";
static char meteo_temp[15] = "--";

static uint16_t theme_color = ILI9341_COLOR_GREEN;
static uint8_t chrono_actif = 0;
static uint32_t chrono_secondes = 0;
static uint8_t luminosite = 100;

#define NB_APPS 6
typedef struct { uint16_t x; uint16_t y; char nom[15]; Page_t page_cible; uint8_t icon_id; } AppButton_t;

static AppButton_t apps[NB_APPS] = {
    {25, 35, "Horloge", PAGE_HORLOGE, 0}, {135, 35, "Notifs", PAGE_NOTIF, 1},
    {245, 35, "Sante", PAGE_SANTE, 2},    {25, 135, "Meteo", PAGE_METEO, 3},
    {135, 135, "NFC", PAGE_NFC, 4},       {245, 135, "Options", PAGE_REGLAGES, 5}
};

/* --- Outils de dessin --- */
static void Ma_Boite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    ILI9341_DrawLine(x, y, x + w, y, color);
    ILI9341_DrawLine(x, y + h, x + w, y + h, color);
    ILI9341_DrawLine(x, y, x, y + h, color);
    ILI9341_DrawLine(x + w, y, x + w, y + h, color);
}

static void Effacer_Zone(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    for(uint16_t i = 0; i < h; i++) ILI9341_DrawLine(x, y + i, x + w, y + i, ILI9341_COLOR_BLACK);
}

static void Dessiner_Icone(uint16_t x, uint16_t y, char* nom, uint8_t id) {
    Ma_Boite(x, y, 50, 50, theme_color);
    Ma_Boite(x + 2, y + 2, 46, 46, theme_color);
    uint16_t cx = x + 25; uint16_t cy = y + 25;
    switch(id) {
        case 0: ILI9341_DrawCircle(cx, cy, 14, theme_color); ILI9341_DrawLine(cx, cy, cx, cy - 8, theme_color); ILI9341_DrawLine(cx, cy, cx + 6, cy + 3, theme_color); break;
        case 1: Ma_Boite(cx - 8, cy - 12, 16, 24, theme_color); ILI9341_DrawCircle(cx, cy + 8, 2, theme_color); ILI9341_DrawLine(cx - 4, cy - 8, cx + 4, cy - 8, theme_color); break;
        case 2: ILI9341_DrawLine(cx, cy - 10, cx, cy + 10, theme_color); ILI9341_DrawLine(cx - 10, cy, cx + 10, cy, theme_color); break;
        case 3: ILI9341_DrawCircle(cx, cy, 6, theme_color); ILI9341_DrawLine(cx, cy - 9, cx, cy - 14, theme_color); ILI9341_DrawLine(cx - 9, cy, cx - 14, cy, theme_color); break;
        case 4: ILI9341_DrawCircle(cx, cy, 6, theme_color); ILI9341_DrawCircle(cx + 4, cy - 4, 12, theme_color); break;
        case 5: ILI9341_DrawCircle(cx, cy, 8, theme_color); ILI9341_DrawLine(cx, cy - 12, cx, cy + 12, theme_color); ILI9341_DrawLine(cx - 12, cy, cx + 12, cy, theme_color); break;
        default: break;
    }
    ILI9341_Puts(x + 2, y + 55, nom, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
}

static void draw_heart_icon(int16_t x, int16_t y, int16_t size, uint16_t color) {
    int16_t r = size / 2;
    ILI9341_DrawFilledCircle(x - r, y, r, color);
    ILI9341_DrawFilledCircle(x + r, y, r, color);
    for(int16_t i = 0; i <= size * 2; i++) ILI9341_DrawLine(x - size + (i/2), y + (i/2), x + size - (i/2), y + (i/2), color);
}

/* --- API --- */
void MENU_init(void) {
    #if USE_XPT2046
        XPT2046_init(); HAL_Delay(50);
    #endif
    ILI9341_Init(); HAL_Delay(50);
    MENU_draw();
}

void MENU_update_time(char* time_str) {
    if (strcmp(text_heure, time_str) == 0) return;
    strcpy(text_heure, time_str);
    if (pageActuelle == PAGE_MENU) {
        Effacer_Zone(125, 5, 80, 10);
        ILI9341_Puts(125, 5, text_heure, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
    }
    if (chrono_actif) chrono_secondes++;
    if (pageActuelle == PAGE_HORLOGE) {
        Effacer_Zone(80, 55, 150, 26);
        ILI9341_Puts(80, 55, text_heure, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
        char chrono_str[15]; sprintf(chrono_str, "%02d:%02d", (int)(chrono_secondes / 60), (int)(chrono_secondes % 60));
        Effacer_Zone(115, 125, 100, 26);
        ILI9341_Puts(115, 125, chrono_str, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
    }
}

void MENU_set_notif(char* texte) { strncpy(text_notif, texte, 49); text_notif[49] = '\0'; if (pageActuelle == PAGE_NOTIF) MENU_draw(); }
void MENU_set_meteo(char* infos) { strncpy(meteo_temp, infos, 14); meteo_temp[14] = '\0'; if (pageActuelle == PAGE_METEO) MENU_draw(); }

void MENU_update_sante(uint16_t adc_val, int bpm, bool is_beating) {
    if (pageActuelle != PAGE_SANTE || mode_sante != 1) return;

    char adc_str[15]; sprintf(adc_str, "RAW: %4d", adc_val);
    ILI9341_Puts(20, 85, adc_str, &Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    char bpm_str[10]; sprintf(bpm_str, "%3d", bpm);
    ILI9341_Puts(160, 85, bpm_str, &Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
    ILI9341_Puts(215, 95, "BPM", &Font_7x10, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK);

    if (is_beating) draw_heart_icon(260, 95, 8, ILI9341_COLOR_RED);
    else draw_heart_icon(260, 95, 6, 0x7BEF);

    static uint16_t x_graph = 12; static uint16_t prev_y = 140;
    uint16_t y_graph = 175 - ((adc_val * 60) / 4096);

    for (uint16_t i = 1; i <= 8; i++) {
        uint16_t erase_x = x_graph + i;
        if (erase_x > 305) erase_x = 12 + (erase_x - 305);
        ILI9341_DrawLine(erase_x, 115, erase_x, 175, ILI9341_COLOR_BLACK);
    }
    if (x_graph > 12) ILI9341_DrawLine(x_graph - 1, prev_y, x_graph, y_graph, ILI9341_COLOR_RED);
    prev_y = y_graph; x_graph++;
    if (x_graph >= 305) x_graph = 12;
}

void MENU_draw(void) {
    ILI9341_Fill(ILI9341_COLOR_BLACK);
    if (pageActuelle == PAGE_MENU) {
        ILI9341_Puts(125, 5, text_heure, &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
        ILI9341_DrawLine(10, 20, 310, 20, theme_color);
        for (int i = 0; i < NB_APPS; i++) Dessiner_Icone(apps[i].x, apps[i].y, apps[i].nom, apps[i].icon_id);
    }
    else {
        ILI9341_Puts(20, 10, nom_selection, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
        ILI9341_DrawLine(20, 40, 300, 40, theme_color);

        switch(pageActuelle) {
            case PAGE_HORLOGE:
                ILI9341_Puts(20, 45, "Heure RTC :", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                ILI9341_Puts(80, 55, text_heure, &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
                Ma_Boite(20, 120, 80, 30, theme_color); ILI9341_Puts(28, 130, chrono_actif ? "STOP" : "START", &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
                Ma_Boite(220, 120, 80, 30, theme_color); ILI9341_Puts(242, 130, "RESET", &Font_7x10, theme_color, ILI9341_COLOR_BLACK);
                break;
            case PAGE_NOTIF: ILI9341_Puts(20, 85, text_notif, &Font_16x26, theme_color, ILI9341_COLOR_BLACK); break;

            case PAGE_SANTE:
                Ma_Boite(10, 45, 135, 30, (mode_sante == 0) ? ILI9341_COLOR_VIOLET : theme_color);
                ILI9341_Puts(35, 55, "PODOMETRE", &Font_7x10, (mode_sante == 0) ? ILI9341_COLOR_VIOLET : theme_color, ILI9341_COLOR_BLACK);
                Ma_Boite(155, 45, 135, 30, (mode_sante == 1) ? ILI9341_COLOR_RED : theme_color);
                ILI9341_Puts(205, 55, "CARDIO", &Font_7x10, (mode_sante == 1) ? ILI9341_COLOR_RED : theme_color, ILI9341_COLOR_BLACK);

                if (mode_sante == 0) {
                    // On remplace le DrawRectangle buggé du driver par notre Ma_Boite sécurisée !
                    Ma_Boite(10, 85, 280, 85, theme_color);
                    char step_str[10]; sprintf(step_str, "%5lu", global_step_count);
                    ILI9341_Puts(90, 115, step_str, &Font_16x26, ILI9341_COLOR_VIOLET, ILI9341_COLOR_BLACK);
                    ILI9341_Puts(180, 125, "PAS", &Font_7x10, ILI9341_COLOR_VIOLET, ILI9341_COLOR_BLACK);
                } else {
                    Ma_Boite(10, 112, 280, 65, theme_color);
                }
                break;

            case PAGE_METEO: ILI9341_Puts(20, 70, meteo_temp, &Font_16x26, theme_color, ILI9341_COLOR_BLACK); break;
            case PAGE_NFC: ILI9341_Puts(20, 100, "Statut : Pret a scanner...", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK); break;
            case PAGE_REGLAGES:
                Ma_Boite(10, 70, 65, 35, ILI9341_COLOR_GREEN); ILI9341_Puts(25, 82, "VERT", &Font_7x10, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
                Ma_Boite(85, 70, 65, 35, ILI9341_COLOR_CYAN);  ILI9341_Puts(100, 82, "BLEU", &Font_7x10, ILI9341_COLOR_CYAN, ILI9341_COLOR_BLACK);
                Ma_Boite(160, 70, 65, 35, ILI9341_COLOR_YELLOW); ILI9341_Puts(170, 82, "JAUNE", &Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
                char lum_str[25]; sprintf(lum_str, "Luminosite : %d %%", luminosite);
                ILI9341_Puts(20, 120, lum_str, &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
                Ma_Boite(20, 140, 280, 20, theme_color);
                for(int i = 0; i < 20; i++) ILI9341_DrawLine(20, 140 + i, 20 + ((luminosite * 280) / 100), 140 + i, theme_color);
                break;
            default: break;
        }
        Ma_Boite(85, 180, 150, 45, theme_color);
        ILI9341_Puts(110, 190, "RETOUR", &Font_16x26, theme_color, ILI9341_COLOR_BLACK);
    }
}

void MENU_handler(void) {
    #if USE_XPT2046
        int16_t x, y;
        if (XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE)) {
            dernier_appui_tactile = HAL_GetTick();

            if (pageActuelle == PAGE_MENU) {
                for (int i = 0; i < NB_APPS; i++) {
                    if (x >= apps[i].x && x <= (apps[i].x + 50) && y >= apps[i].y && y <= (apps[i].y + 70)) {
                        strcpy(nom_selection, apps[i].nom); pageActuelle = apps[i].page_cible; MENU_draw(); HAL_Delay(300); return;
                    }
                }
            }
            else {
                if (x >= 85 && x <= 235 && y >= 180 && y <= 225) { pageActuelle = PAGE_MENU; MENU_draw(); HAL_Delay(300); return; }

                if (pageActuelle == PAGE_SANTE) {
                    if (x >= 10 && x <= 145 && y >= 45 && y <= 75) { mode_sante = 0; MENU_draw(); HAL_Delay(200); return; }
                    if (x >= 155 && x <= 290 && y >= 45 && y <= 75) { mode_sante = 1; MENU_draw(); HAL_Delay(200); return; }
                }

                if (pageActuelle == PAGE_HORLOGE) {
                    if (x >= 20 && x <= 100 && y >= 120 && y <= 150) { chrono_actif = !chrono_actif; MENU_draw(); HAL_Delay(300); return; }
                    if (x >= 220 && x <= 300 && y >= 120 && y <= 150) { chrono_actif = 0; chrono_secondes = 0; MENU_draw(); HAL_Delay(300); return; }
                }

                if (pageActuelle == PAGE_REGLAGES) {
                    if (x >= 10 && x <= 75 && y >= 70 && y <= 105) { theme_color = ILI9341_COLOR_GREEN; MENU_draw(); HAL_Delay(200); return; }
                    if (x >= 85 && x <= 150 && y >= 70 && y <= 105) { theme_color = ILI9341_COLOR_CYAN; MENU_draw(); HAL_Delay(200); return; }
                    if (x >= 160 && x <= 225 && y >= 70 && y <= 105) { theme_color = ILI9341_COLOR_YELLOW; MENU_draw(); HAL_Delay(200); return; }
                    if (x >= 20 && x <= 300 && y >= 130 && y <= 170) {
                        int nouveau = ((x - 20) * 100) / 280;
                        if (nouveau < 5) nouveau = 5; if (nouveau > 100) nouveau = 100;
                        if (luminosite != nouveau) { luminosite = nouveau; Backlight_Set(luminosite); MENU_draw(); }
                    }
                }
            }
        }
    #endif

    // Rafraîchissement non-bloquant des pas
    static uint32_t dernier_refresh_pas = 0;
    if (pageActuelle == PAGE_SANTE && mode_sante == 0 && (HAL_GetTick() - dernier_refresh_pas > 250)) {
        dernier_refresh_pas = HAL_GetTick();
        char step_refresh[10];
        sprintf(step_refresh, "%5lu", global_step_count);
        ILI9341_Puts(90, 115, step_refresh, &Font_16x26, ILI9341_COLOR_VIOLET, ILI9341_COLOR_BLACK);
    }
}

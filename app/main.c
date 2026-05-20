/**
 * @file    main.c
 * @brief   Ordonnanceur principal - Gestion du RTC matériel et décodage Bluetooth.
 */

#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_uart.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "menu.h"
#include "buzzer.h"
#include <stdio.h>
#include <string.h>

void Bluetooth_Task(void);
void Sensors_Send_Task(void);
void Clock_Task(void);
void Activer_Horloge_Materielle(void);

static char buffer_reception[100];
static int index_rec = 0;
static uint32_t dernier_envoi_capteurs = 0;
static uint32_t dernier_envoi_horloge = 0;

RTC_HandleTypeDef hrtc;

int main(void) {
    HAL_Init();
    SystemClock_Config();

    /* Initialisation du Buzzer (PA1 via TIMER6) */
    Buzzer_Init();

    /* Activation de la source d'oscillation matérielle pour le calendrier */
    Activer_Horloge_Materielle();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    HAL_RTC_Init(&hrtc);

    MENU_init();
    BSP_UART_init(UART1_ID, 9600);

    while (1) {
        MENU_handler();
        Bluetooth_Task();
        Clock_Task();
        Sensors_Send_Task();
    }
}

/**
 * @brief Déverrouille l'accès sauvegarde et démarre l'oscillateur interne LSI pour le RTC.
 */
void Activer_Horloge_Materielle(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    HAL_PWR_EnableBkUpAccess();

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    __HAL_RCC_RTC_ENABLE();
}

void Clock_Task(void) {
    if (HAL_GetTick() - dernier_envoi_horloge > 1000) {
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        char heure_str[15];
        sprintf(heure_str, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);

        MENU_update_time(heure_str);
        dernier_envoi_horloge = HAL_GetTick();
    }
}

void Bluetooth_Task(void) {
    if (BSP_UART_data_ready(UART1_ID)) {
        char lettre = BSP_UART_getc(UART1_ID);

        if (lettre == '\n' || lettre == '\r') {
            if (index_rec > 0) {
                buffer_reception[index_rec] = '\0';

                // Protocole 'N:' -> Notification
                if (strncmp(buffer_reception, "N:", 2) == 0) {
                    Buzzer_Beep(150); // Bip rallongé à 150 ms sur PA1
                    MENU_set_notif(&buffer_reception[2]);
                }
                // Protocole 'H:' -> Mise à l'heure
                else if (strncmp(buffer_reception, "H:", 2) == 0) {
                    int h = 0, m = 0;
                    if (sscanf(&buffer_reception[2], "%d:%d", &h, &m) == 2) {
                        RTC_TimeTypeDef sTime = {0};
                        sTime.Hours = h;
                        sTime.Minutes = m;
                        sTime.Seconds = 0;
                        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                    }
                }
                // Protocole 'M:' -> Météo dynamique
                else if (strncmp(buffer_reception, "M:", 2) == 0) {
                    char etat[15];
                    char temp[10];
                    if (sscanf(&buffer_reception[2], "%14[^:]:%9s", etat, temp) == 2) {
                        MENU_set_meteo(etat, temp);
                    }
                }
                index_rec = 0;
            }
        } else if (index_rec < 99) {
            buffer_reception[index_rec++] = lettre;
        }
    }
}

void Sensors_Send_Task(void) {
    if (HAL_GetTick() - dernier_envoi_capteurs > 2000) {
        char buffer_tx[50];
        int bpm = 72;
        int temp = 37;

        sprintf(buffer_tx, "BPM:%d|Temp:%d C\n", bpm, temp);

        for (int i = 0; i < strlen(buffer_tx); i++) {
            BSP_UART_putc(UART1_ID, buffer_tx[i]);
        }
        dernier_envoi_capteurs = HAL_GetTick();
    }
}

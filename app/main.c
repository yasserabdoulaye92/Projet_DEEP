/**
 * @file    main.c
 * @author  Yasser Abdoulaye
 * @brief   Système principal de la Smartwatch STM32G4 — Super-boucle.
 *
 * @details Architecture de la super-boucle :
 *  - Bluetooth_Task() : réception série et dispatch des commandes BT.
 *  - Clock_Task()     : lecture RTC et mise à jour de l'écran (5 Hz).
 *  - Health_Task()    : acquisition ADC + algorithme BPM (50 Hz).
 *  - Podometer_Update(): détection de pas via MPU6050 (50 Hz).
 *  - MENU_handler()   : navigation tactile (sauf en mode Snake).
 *
 * @section Protocole Protocole Bluetooth
 *  - "N:<texte>\\n"  : notification à afficher + bip buzzer.
 *  - "H:HH:MM\\n"   : synchronisation de l'horloge RTC.
 *  - "M:<texte>\\n"  : mise à jour des données météo.
 *
 * @section RTC Horloge temps réel
 *  Utilise le driver BSP de l'école (stm32g4_rtc.c) cadencé sur le LSI
 *  (oscillateur interne ~32 kHz) : fonctionne sans quartz externe et active
 *  correctement l'horloge d'accès aux registres (__HAL_RCC_RTCAPB_CLK_ENABLE,
 *  indispensable sur STM32G4). La lecture passe par
 *  BSP_RTC_get_time_and_date() qui lit heure PUIS date pour déverrouiller
 *  les registres fantômes (sinon l'heure se fige).
 */

#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_uart.h"
#include "stm32g4_adc.h"
#include "stm32g4_rtc.h"
#include "hw-827.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "menu.h"
#include "buzzer.h"
#include "podometer.h"
#include "snake.h"
#include <stdio.h>
#include <string.h>

/* --- Variables Globales --- */
volatile uint32_t dernier_appui_tactile = 0; ///< Déclaration globale essentielle pour le fichier menu.c
static char buffer_reception[100];
static int index_rec = 0;
static uint32_t dernier_envoi_horloge = 0;
static uint32_t dernier_envoi_sante = 0;
static uint32_t timerCount = 0;
static int current_bpm = 0;
static bool snake_is_running = false;

/* --- Prototypes --- */
void Bluetooth_Task(void);
void Clock_Task(void);
void Health_Task(void);
void Snake_Task(void);

/**
 * @brief  Point d'entrée principal — initialise le matériel puis lance la super-boucle.
 * @return Ne retourne jamais (boucle infinie).
 */
int main(void) {
    HAL_Init();
    SystemClock_Config();

    Buzzer_Init();
    Buzzer_Beep(150); /* Bip de démarrage */

    /* --- HORLOGE RTC (driver BSP, source LSI) --- */
    /* Le domaine de sauvegarde survit au reflash : si une ancienne config
     * (ex: LSE) y traîne, le changement de source ferait attendre ou échouer
     * le driver au boot. On repart d'un domaine propre si la source n'est
     * pas déjà le LSI. */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    if (READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL) != RCC_RTCCLKSOURCE_LSI) {
        __HAL_RCC_BACKUPRESET_FORCE();
        __HAL_RCC_BACKUPRESET_RELEASE();
    }

    /* Le BSP sélectionne le LSI comme source mais ne l'allume pas : on
     * l'active explicitement ici avant d'initialiser la RTC. */
    RCC_OscInitTypeDef osc_lsi = {0};
    osc_lsi.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    osc_lsi.LSIState = RCC_LSI_ON;
    HAL_RCC_OscConfig(&osc_lsi);

    BSP_RTC_init();

    #if USE_ADC
    BSP_ADC_init();
    #endif

    /* --- CONFIGURATION DES BOUTONS --- */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio_btn = {0};
    gpio_btn.Mode = GPIO_MODE_INPUT;
    gpio_btn.Pull = GPIO_PULLUP;
    gpio_btn.Speed = GPIO_SPEED_FREQ_LOW;

    /* --- Boutons Snake (croix directionnelle de la carte DEEP Purple) ---
     * Câblage : pastille de chaque bouton -> broche Nucleo (fil à souder).
     *   ButtonU (HAUT)   -> PB4
     *   ButtonD (BAS)    -> PB6
     *   ButtonL (GAUCHE) -> PB0
     *   ButtonR (DROITE) -> PA12
     * Actifs à l'état bas (pull-up 10k + condo 100nF déjà sur la carte).
     * Pins interdits : PA0 (ADC pouls), PB7/PA15 (I2C MPU6050),
     * PA4-PA8/PB3/PB5/PA11 (TFT+tactile), PA9/PA10 (Bluetooth). */
    gpio_btn.Pin = GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOB, &gpio_btn);

    gpio_btn.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &gpio_btn);

    MENU_init();
    BSP_UART_init(UART1_ID, 9600);
    Podometer_Init();

    /* --- SUPER-BOUCLE (Mode Stable sans Veille) --- */
    while (1) {

        Bluetooth_Task();
        Clock_Task();
        Health_Task();
        Podometer_Update();

        if (MENU_Get_Page() == PAGE_SNAKE) {
            if (!snake_is_running) {
                Snake_Init();
                snake_is_running = true;
            }
            Snake_Task();

            if (Snake_IsGameOver() && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET) {
                snake_is_running = false;
                MENU_Set_Page(PAGE_MENU);
                HAL_Delay(200);
            }
        }
        else {
            MENU_handler();
        }
    }
}

/**
 * @brief  Tâche Snake — avance le jeu et redessine à la cadence de Snake_GetDelay().
 * @note   Appelée uniquement lorsque PAGE_SNAKE est active.
 */
void Snake_Task(void) {
    static uint32_t chrono_snake = 0;
    if (HAL_GetTick() - chrono_snake >= Snake_GetDelay()) {
        chrono_snake = HAL_GetTick();
        Snake_Update();
        Snake_Draw();
    }
}

/**
 * @brief  Tâche santé — lit l'ADC toutes les 20 ms et calcule le BPM.
 *
 * @details L'algorithme mesure le temps entre deux fronts montants du signal
 *          cardiaque. BPM = 60 000 / intervalle_ms. Les valeurs hors
 *          [40, 220] BPM sont rejetées pour filtrer les artefacts de mouvement.
 */
void Health_Task(void) {
    if (HAL_GetTick() - dernier_envoi_sante >= 20) {
        dernier_envoi_sante = HAL_GetTick();
        uint16_t adcRaw = 0;

        #if USE_ADC
        adcRaw = BSP_ADC_getValue(ADC_1); /* ADC_1 = PA0 (capteur HW827) */
        #endif

        bool isBeating = false;
        if (HW827_HeartbeatDetected(adcRaw, 20)) {
            if (timerCount > 0) {
                int calc_bpm = 60000 / timerCount;
                if (calc_bpm >= 40 && calc_bpm <= 220) current_bpm = calc_bpm;
            }
            timerCount = 0;
            isBeating = true;
        }
        MENU_update_sante(adcRaw, current_bpm, isBeating);
        timerCount += 20;
    }
}

/**
 * @brief  Tâche Bluetooth — lit les caractères UART1 et dispatche les commandes.
 *
 * @details Commandes supportées (terminées par '\\n' ou '\\r') :
 *  - "N:<msg>"  : notification → MENU_set_notif() + Buzzer_Beep(300).
 *  - "H:HH:MM"  : sync RTC → BSP_RTC_set_time().
 *  - "M:<info>" : météo → MENU_set_meteo().
 */
void Bluetooth_Task(void) {
    if (BSP_UART_data_ready(UART1_ID)) {
        char lettre = BSP_UART_getc(UART1_ID);
        if (lettre == '\n' || lettre == '\r') {
            if (index_rec > 0) {
                buffer_reception[index_rec] = '\0';
                if (strncmp(buffer_reception, "N:", 2) == 0) {
                    MENU_set_notif(&buffer_reception[2]);
                    Buzzer_Beep(300); /* Bip à chaque notification Bluetooth */
                }
                else if (strncmp(buffer_reception, "H:", 2) == 0) {
                    int h = 0, m = 0;
                    if (sscanf(&buffer_reception[2], "%d:%d", &h, &m) == 2) {
                        RTC_TimeTypeDef sTime = {0};
                        sTime.Hours = h; sTime.Minutes = m; sTime.Seconds = 0;
                        BSP_RTC_set_time(&sTime);
                    }
                }
                else if (strncmp(buffer_reception, "M:", 2) == 0) MENU_set_meteo(&buffer_reception[2]);
                index_rec = 0;
            }
        } else if (index_rec < 99) buffer_reception[index_rec++] = lettre;
    }
}

/**
 * @brief  Tâche horloge — lit le RTC et met à jour l'écran si l'heure a changé.
 * @note   BSP_RTC_get_time_and_date lit l'heure PUIS la date : obligatoire
 *         pour déverrouiller les registres fantômes du RTC, sinon l'heure
 *         reste figée à la première lecture (gotcha STM32).
 */
void Clock_Task(void) {
    if (HAL_GetTick() - dernier_envoi_horloge >= 200) {
        dernier_envoi_horloge = HAL_GetTick();
        RTC_TimeTypeDef sTime;
        RTC_DateTypeDef sDate;
        BSP_RTC_get_time_and_date(&sTime, &sDate);
        char time_str[15];
        sprintf(time_str, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
        MENU_update_time(time_str);
    }
}

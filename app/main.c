/**
 * @file    main.c
 * @brief   Ordonnanceur principal - Gestion d'énergie (Veille Automatique), RTC, Bluetooth, ECG, Rétroéclairage.
 */

#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_uart.h"
#include "stm32g4_adc.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "menu.h"
#include "buzzer.h"
#include "hw-827.h"
#include <stdio.h>
#include <string.h>

/* --- Prototypes --- */
void Bluetooth_Task(void);
void Sensors_Send_Task(void);
void Clock_Task(void);
void Health_Task(void);
void PowerManagement_Task(void); // Nouvelle tâche de gestion d'énergie
void Activer_Horloge_Materielle(void);
void Backlight_Init(void);
void Backlight_Set(uint8_t pourcentage);
void Entrer_Mode_Veille(void);
void WakeUp_Init(void);

/* --- Variables Globales --- */
static char buffer_reception[100];
static int index_rec = 0;
static uint32_t dernier_envoi_capteurs = 0;
static uint32_t dernier_envoi_horloge = 0;
static uint32_t dernier_envoi_sante = 0;

static uint32_t timerCount = 0;
static int current_bpm = 0;

// Variables pour la mise en veille
volatile uint32_t dernier_appui_tactile = 0;
#define DELAI_VEILLE_MS 10000 // 10 secondes d'inactivité avant veille

RTC_HandleTypeDef hrtc;
TIM_HandleTypeDef htim4; // PWM Luminosité sur PB6

int main(void) {
    HAL_Init();
    SystemClock_Config();

    /* Initialisation Matérielle */
    Buzzer_Init();
    Activer_Horloge_Materielle();
    WakeUp_Init(); // Active l'interruption sur PB5 pour le réveil

    #if USE_ADC
    BSP_ADC_init();
    #endif

    /* Configuration RTC */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 249;
    HAL_RTC_Init(&hrtc);

    RTC_TimeTypeDef sTimeInit = {0};
    sTimeInit.Hours = 12;
    HAL_RTC_SetTime(&hrtc, &sTimeInit, RTC_FORMAT_BIN);
    RTC_DateTypeDef sDateInit = {0};
    sDateInit.Year = 24;
    HAL_RTC_SetDate(&hrtc, &sDateInit, RTC_FORMAT_BIN);

    /* Interface & Communication */
    MENU_init();
    BSP_UART_init(UART1_ID, 9600);

    Backlight_Init();
    Backlight_Set(100);

    dernier_appui_tactile = HAL_GetTick(); // On initialise le chrono d'inactivité

    /* Super-Boucle Asynchrone */
    while (1) {
        MENU_handler();
        Bluetooth_Task();
        Clock_Task();
        Health_Task();
        Sensors_Send_Task();
        PowerManagement_Task(); // On vérifie s'il est l'heure de dormir
    }
}

/* --- GESTION DE L'ÉNERGIE (MODE VEILLE & RÉVEIL) --- */

void WakeUp_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Le XPT2046 met la broche à 0V au toucher
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Priorité très haute pour être sûr de réveiller le STM32
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_5) {
        dernier_appui_tactile = HAL_GetTick();
    }
}
void PowerManagement_Task(void) {
    // Si on dépasse le délai d'inactivité
    if ((HAL_GetTick() - dernier_appui_tactile) > DELAI_VEILLE_MS) {
        Entrer_Mode_Veille();
    }
}

void Entrer_Mode_Veille(void) {
    // 1. Éteindre le rétroéclairage et la matrice graphique
    Backlight_Set(0);
    ILI9341_SendCommand(0x10); // Commande "Sleep In" de l'écran
    HAL_Delay(50);

    // 2. Préparer le microcontrôleur au sommeil
    HAL_SuspendTick(); // Arrête le compteur de millisecondes

    // 3. Zzz... Entrée en mode STOP (Le code fige ici jusqu'au toucher)
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // ==========================================
    // RÉVEIL (On reprend ici quand PB5 est touché)
    // ==========================================

    // 4. Relancer le système (Le mode STOP désactive la PLL, il faut la relancer)
    SystemClock_Config();
    HAL_ResumeTick();

    // 5. Rallumer l'écran
    ILI9341_SendCommand(0x11); // Commande "Sleep Out"
    HAL_Delay(120); // La puce graphique a besoin de temps pour redémarrer
    Backlight_Set(100);

    // 6. Réinitialiser le chrono d'inactivité
    dernier_appui_tactile = HAL_GetTick();
}


/* --- GESTION DU RÉTROÉCLAIRAGE (PB6 - TIMER 4) --- */
void Backlight_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 170 - 1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 100 - 1;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim4);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}

void Backlight_Set(uint8_t pourcentage) {
    if (pourcentage > 100) pourcentage = 100;
    uint32_t pulse = 100 - pourcentage;
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pulse);
}

/* --- TÂCHES ASYNCHRONES --- */
void Activer_Horloge_Materielle(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BACKUPRESET_FORCE();
    __HAL_RCC_BACKUPRESET_RELEASE();
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

void Health_Task(void) {
    if (HAL_GetTick() - dernier_envoi_sante >= 20) {
        dernier_envoi_sante = HAL_GetTick();
        uint16_t adcRaw = 0;
        #if USE_ADC
        adcRaw = BSP_ADC_getValue(ADC_1);
        #endif
        bool isBeating = false;
        if (HW827_HeartbeatDetected(adcRaw, 20)) {
            if (timerCount > 0) {
                int calc_bpm = 60000 / timerCount;
                if (calc_bpm >= BPM_MIN && calc_bpm <= BPM_MAX) current_bpm = calc_bpm;
            }
            timerCount = 0;
            isBeating = true;
        }
        MENU_update_sante(adcRaw, current_bpm, isBeating);
        timerCount += 20;
    }
}

void Bluetooth_Task(void) {
    if (BSP_UART_data_ready(UART1_ID)) {
        char lettre = BSP_UART_getc(UART1_ID);
        // On simule une "activité" Bluetooth pour repousser la veille si on reçoit des infos
        dernier_appui_tactile = HAL_GetTick();

        if (lettre == '\n' || lettre == '\r') {
            if (index_rec > 0) {
                buffer_reception[index_rec] = '\0';
                if (strncmp(buffer_reception, "N:", 2) == 0) { Buzzer_Beep(150); MENU_set_notif(&buffer_reception[2]); }
                else if (strncmp(buffer_reception, "H:", 2) == 0) {
                    int h = 0, m = 0;
                    if (sscanf(&buffer_reception[2], "%d:%d", &h, &m) == 2) {
                        RTC_TimeTypeDef sTime = {0};
                        sTime.Hours = h; sTime.Minutes = m; sTime.Seconds = 0;
                        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                    }
                }
                else if (strncmp(buffer_reception, "M:", 2) == 0) MENU_set_meteo(&buffer_reception[2]);
                index_rec = 0;
            }
        } else if (index_rec < 99) buffer_reception[index_rec++] = lettre;
    }
}

void Sensors_Send_Task(void) {
    if (HAL_GetTick() - dernier_envoi_capteurs > 2000) {
        char buffer_tx[50];
        sprintf(buffer_tx, "BPM:%d|Temp:37 C\n", current_bpm);
        for (int i = 0; i < strlen(buffer_tx); i++) BSP_UART_putc(UART1_ID, buffer_tx[i]);
        dernier_envoi_capteurs = HAL_GetTick();
    }
}

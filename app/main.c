#include "config.h"
#include "stm32g4_sys.h"
#include "stm32g4_uart.h"
#include "stm32g4_adc.h"
#include "hw-827.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "menu.h"
#include "buzzer.h"
#include "podometer.h"
#include <stdio.h>
#include <string.h>

volatile uint32_t dernier_appui_tactile = 0;
static char buffer_reception[100];
static int index_rec = 0;
static uint32_t dernier_envoi_horloge = 0;

static uint32_t dernier_envoi_sante = 0;
static uint32_t timerCount = 0;
static int current_bpm = 0;

RTC_HandleTypeDef hrtc;

void Bluetooth_Task(void);
void Clock_Task(void);
void Health_Task(void);
void Activer_Horloge_Materielle(void);
void Backlight_Set(uint8_t pourcentage);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    Buzzer_Init();
    Activer_Horloge_Materielle();

    #if USE_ADC
    BSP_ADC_init();
    #endif

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    HAL_RTC_Init(&hrtc);

    MENU_init();
    BSP_UART_init(UART1_ID, 9600);

    // Initialisation du Podomètre (via ton driver école)
    Podometer_Init();

    while (1) {
        MENU_handler();
        Bluetooth_Task();
        Clock_Task();
        Health_Task();

        // Comptage des pas
        Podometer_Update();
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
                if (calc_bpm >= 40 && calc_bpm <= 220) current_bpm = calc_bpm;
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
        if (lettre == '\n' || lettre == '\r') {
            if (index_rec > 0) {
                buffer_reception[index_rec] = '\0';
                if (strncmp(buffer_reception, "N:", 2) == 0) MENU_set_notif(&buffer_reception[2]);
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

void Clock_Task(void) {
    if (HAL_GetTick() - dernier_envoi_horloge > 1000) {
        dernier_envoi_horloge = HAL_GetTick();
        RTC_TimeTypeDef sTime; HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        char time_str[15]; sprintf(time_str, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
        MENU_update_time(time_str);
    }
}

void Activer_Horloge_Materielle(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void Backlight_Set(uint8_t pourcentage) { (void)pourcentage; }

/**
 * @file    config.h
 * @brief   Configuration globale du projet montre sur carte STM32G431 (ESEO).
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32g4xx_hal.h"

/* --- Configuration GPIO --- */
#define LED_GREEN_PIN       GPIO_PIN_15
#define LED_GREEN_GPIO      GPIOB

/* --- Configuration UART --- */
#define UART2_ON_PA3_PA2
#define UART1_ON_PA10_PA9

/* --- Activation des Périphériques --- */
#define USE_BSP_TIMER       1
#define USE_BSP_EXTIT       1
#define USE_BUZZER          1
#define USE_RTC             1

/* --- Configuration ADC / DAC --- */
#define USE_ADC             0
#define USE_IN1             1 // PA0 (Futur MCP9701)
#define USE_IN2             0 // Désactivé car PA1 est utilisé par le Buzzer
#define USE_IN3             1 // PA6
#define USE_IN4             1 // PA7
#define USE_IN13            1 // PA5
#define USE_IN17            1 // PA4
#define USE_DAC             0

/* --- Configuration Écran TFT ILI9341 & Tactile --- */
#define USE_ILI9341         1
#if USE_ILI9341
    #define USE_XPT2046     1
    #define USE_FONT7x10    1
    #define USE_FONT11x18   0
    #define USE_FONT16x26   1
#endif

#define USE_EPAPER          0
#define USE_WS2812          0

/* --- Capteurs & Actionneurs (Désactivés) --- */
#define USE_MPU6050         0
#define USE_APDS9960        0
#define USE_BMP180          0
#define USE_BH1750FVI       0
#define USE_DHT11           0
#define USE_DS18B20         0
#define USE_YX6300          0
#define USE_MATRIX_KEYBOARD 0
#define USE_HCSR04          0
#define USE_GPS             0
#define USE_LD19            0
#define USE_NFC03A1         0
#define USE_VL53L0          0
#define USE_MCP23017        0
#define USE_MCP23S17        0
#define USE_SD_CARD         0
#define USE_MOTOR_DC        0

/* --- Gestionnaires de Bus --- */
#ifndef USE_I2C
    #define USE_I2C         0
#endif
#define I2C_TIMEOUT         5

#if USE_ILI9341 || USE_SD_CARD || USE_MCP23S17 || USE_EPAPER
    #define USE_SPI         1
#else
    #ifndef USE_SPI
        #define USE_SPI     0
    #endif
#endif

#define USE_TESTBOARD       0

#endif /* CONFIG_H_ */

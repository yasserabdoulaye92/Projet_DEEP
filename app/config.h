/**
 *******************************************************************************
 * @file    config.h
 * @author  jjo / Adapté pour Projet Montre
 * @date    Mar 29, 2024
 * @brief   Fichier principal de configuration de votre projet sur carte Nucléo STM32G431.
 * Permet d'activer les différents modules logiciels à votre disposition.
 *******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONFIG_H_
#define CONFIG_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Defines -------------------------------------------------------------------*/
#define LED_GREEN_PIN       GPIO_PIN_8
#define LED_GREEN_GPIO      GPIOB

#define UART2_ON_PA3_PA2
#define UART1_ON_PA10_PA9

#define USE_BSP_TIMER       1
#define USE_BSP_EXTIT       1

/* ACTIVATION DE L'HORLOGE INTERNE */
#define USE_RTC             1 // Mis à 1 pour faire fonctionner l'horloge

#define USE_ADC             0
    /* Configuration pour activer les entrées analogiques souhaitées */
    #define USE_IN1     1 //Broche correspondante: PA0
    #define USE_IN2     1 //Broche correspondante: PA1
    #define USE_IN3     1 //Broche correspondante: PA6
    #define USE_IN4     1 //Broche correspondante: PA7
    #define USE_IN13    1 //Broche correspondante: PA5
    #define USE_IN17    1 //Broche correspondante: PA4

#define USE_DAC             0

/*------------------Afficheurs------------------*/
#define USE_ILI9341         1 // Écran TFT activé [cite: 10, 12]
#if USE_ILI9341
    #define USE_XPT2046     1 // Tactile activé
    #define USE_FONT7x10    1 // Petite police [cite: 11, 12]
    #define USE_FONT11x18   0
    #define USE_FONT16x26   1 // Grande police pour l'heure [cite: 11, 12]
#endif

#define USE_EPAPER          0
#define USE_WS2812          0

/*------------------Capteurs (Tous désactivés pour le test solo)------------------*/
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

/*------------------Expanders------------------*/
#define USE_MCP23017        0
#define USE_MCP23S17        0
#define USE_SD_CARD         0

/*------------------Actionneurs------------------*/
#define USE_MOTOR_DC        0

/*------------------Périphériques------------------*/

/* Gestion automatique de l'I2C */
#if USE_MLX90614 || USE_MPU6050 || USE_APDS9960 || USE_BH1750FVI || USE_BMP180 || USE_MCP23017 || USE_VL53L0
    #define USE_I2C         1
#else
    #ifndef USE_I2C
        #define USE_I2C     0 // Désactivé car aucun capteur I2C n'est utilisé
    #endif
#endif
#define I2C_TIMEOUT         5 //ms

/* Gestion automatique du SPI (Activé pour l'écran) */
#if USE_ILI9341 || USE_SD_CARD || USE_MCP23S17 || USE_EPAPER
    #define USE_SPI         1 // Activé automatiquement pour l'écran TFT
#else
    #ifndef USE_SPI
        #define USE_SPI         0
    #endif
#endif

#define USE_TESTBOARD       0

#endif /* CONFIG_H_ */

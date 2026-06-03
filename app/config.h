#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32g4xx_hal.h"
#include <stdbool.h>

/* --- CORRECTION MAGIQUE DU DRIVER ESEO --- */
#define I2C_TIMEOUT 100

#define LED_GREEN_PIN       GPIO_PIN_15
#define LED_GREEN_GPIO      GPIOB

#define UART2_ON_PA3_PA2
#define UART1_ON_PA10_PA9

#define USE_BSP_TIMER       1
#define USE_BSP_EXTIT       1
#define USE_BUZZER          1
#define USE_RTC             1

/* --- ON RÉACTIVE LES DRIVERS OFFICIELS --- */
#define USE_I2C             1
#define USE_BSP_I2C         1
#define USE_MPU6050         1

#define USE_ADC             1
#define USE_IN1             1
#define USE_IN2             0
#define USE_IN3             1
#define USE_IN4             1
#define USE_IN13            1
#define USE_IN17            1
#define USE_DAC             0

#define USE_ILI9341         1
#if USE_ILI9341
    #define USE_XPT2046     1
    #define USE_FONT7x10    1
    #define USE_FONT11x18   0
    #define USE_FONT16x26   1
#endif

#define USE_EPAPER          0
#define USE_WS2812          0
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

#endif /* CONFIG_H_ */

/**
******************************************************************************
* @file main.c
* @brief Podomètre minimal — STM32G431 + ILI9341 (SPI) + MPU6050 (I2C)
*
* Fonctionnement :
* - Lecture accéléromètre MPU6050 toutes les 20 ms
* - Détection de pas par seuillage sur la magnitude du vecteur accél.
* - Affichage du compteur sur écran TFT ILI9341 toutes les 200 ms
******************************************************************************
*/
 
/* ==================================================================
Includes
================================================================== */
#include "config.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "stm32g4_mpu6050.h"
#include "stm32g4xx_hal.h"
#include <math.h>
#include <stdio.h>
#include <inttypes.h>
 
/* ==================================================================
Prototypes HAL (générés par CubeMX — ne pas modifier)
================================================================== */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
 
/* ==================================================================
Variables privées
================================================================== */
static MPU6050_t mpu;
 
/* --- Compteur de pas --- */
static uint32_t stepCount = 0;
static float lastMagnitude = 0.0f;
static uint8_t peakDetected = 0;
static uint32_t lastStepTime = 0;
 
/* --- Timings --- */
static uint32_t lastReadTick = 0;
static uint32_t lastDisplayTick = 0;
static uint32_t lastStepShown = 0xFFFFFFFF;
 
/* ==================================================================
Dessin initial de l'écran (appelé une seule fois)
================================================================== */
static void DrawScreen(void)
{
ILI9341_FillScreen(COLOR_BG);
 
/* Titre */
ILI9341_WriteString(80, 10,
"PODOMETRE",
Font_16x26,
COLOR_TITLE, COLOR_BG);
 
/* Ligne séparatrice */
ILI9341_DrawHLine(10, 50, SCREEN_WIDTH - 20, COLOR_ACCENT);
 
/* Label fixe */
ILI9341_WriteString(50, 80,
"Nombre de pas :",
Font_11x18,
COLOR_TEXT, COLOR_BG);
 
/* Cadre du compteur */
ILI9341_DrawRect(80, 110, 160, 60, COLOR_ACCENT);
}
 
/* ==================================================================
Mise à jour de la zone compteur uniquement (pas de redraw complet)
================================================================== */
static void RefreshCount(void)
{
char buf[12];
snprintf(buf, sizeof(buf), "%6" PRIu32, stepCount);
 
/* Efface l'ancienne valeur */
ILI9341_FillRect(82, 112, 156, 56, COLOR_BG);
 
/* Affiche la nouvelle */
ILI9341_WriteString(90, 125,
buf,
Font_16x26,
COLOR_SUCCESS, COLOR_BG);
}
 
/* ==================================================================
Détection de pas (appelée à chaque lecture MPU)
================================================================== */
static void DetectStep(float ax, float ay, float az)
{
float mag = sqrtf(ax*ax + ay*ay + az*az);
uint32_t now = HAL_GetTick();
 
/* Front montant : magnitude passe au-dessus du seuil */
if (mag > PEDO_THRESHOLD && lastMagnitude <= PEDO_THRESHOLD)
peakDetected = 1;
 
/* Front descendant : on valide le pas */
if (peakDetected && mag <= PEDO_THRESHOLD)
{
if ((now - lastStepTime) >= PEDO_MIN_INTERVAL)
{
stepCount++;
lastStepTime = now;
}
peakDetected = 0;
}
 
lastMagnitude = mag;
}
 
/* ==================================================================
main()
================================================================== */
int main(void)
{
/* --- Init HAL & périphériques --- */
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();
MX_I2C1_Init(); /* MPU6050 sur I2C */
MX_SPI1_Init(); /* ILI9341 sur SPI */
 
/* --- Init écran --- */
ILI9341_Init();
ILI9341_SetRotation(SCREEN_ROTATION);
 
/* --- Init MPU6050 --- */
MPU6050_Init(&mpu);
MPU6050_SetAccelRange(&mpu, MPU6050_ACCEL_RANGE);
 
/* --- Dessin initial --- */
DrawScreen();
RefreshCount();
 
/* ==============================================================
Boucle principale
============================================================== */
while (1)
{
uint32_t now = HAL_GetTick();
 
/* Lecture MPU6050 toutes les 20 ms (~50 Hz) */
if ((now - lastReadTick) >= PEDO_READ_PERIOD)
{
lastReadTick = now;
MPU6050_Read(&mpu);
DetectStep(mpu.Ax, mpu.Ay, mpu.Az);
}
 
/* Rafraîchissement affichage toutes les 200 ms
ou dès qu'un nouveau pas est détecté */
if ((now - lastDisplayTick) >= PEDO_DISPLAY_PERIOD
|| stepCount != lastStepShown)
{
lastDisplayTick = now;
lastStepShown = stepCount;
RefreshCount();
}
}
}

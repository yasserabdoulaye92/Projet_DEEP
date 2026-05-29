/**
 ******************************************************************************
 * @file    podometer.h
 * @brief   Interface publique du podomètre
 ******************************************************************************
 */

#ifndef PODOMETER_H
#define PODOMETER_H

#include <stdint.h>

/* ==================================================================
   Structure interne du podomètre
   ================================================================== */
typedef struct {
    uint32_t stepCount;
    float    threshold;
    float    lastMagnitude;
    uint8_t  peakDetected;
    uint32_t lastStepTime;
    uint32_t minStepInterval;
} StepCounter;

/* ==================================================================
   API publique
   ================================================================== */
void     Podometer_Init(void);
void     Podometer_Reset(void);
void     Podometer_Update(void);        /* À appeler dans while(1)          */
void     Podometer_DrawScreen(void);    /* Dessin initial complet (1x)      */
void     Podometer_RefreshDisplay(void);/* Rafraîchit uniquement les zones  */
uint32_t Podometer_GetStepCount(void);

#endif /* PODOMETER_H */

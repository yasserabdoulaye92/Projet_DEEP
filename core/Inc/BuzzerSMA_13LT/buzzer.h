#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32g4xx_hal.h"

// Définitions des broches (Facile à changer ici si besoin)
#define BUZZER_PIN  GPIO_PIN_5
#define BUZZER_GPIO GPIOA

// Prototypes des fonctions
void Buzzer_Init(void);
void Buzzer_Tone(uint32_t duration_ms);

#endif /* __BUZZER_H */

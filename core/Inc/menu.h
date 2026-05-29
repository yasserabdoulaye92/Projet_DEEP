#ifndef MENU_H
#define MENU_H

#include <stdint.h>

/* ------------------------------------------------------------------
   États de l'application (machine d'états)
   ------------------------------------------------------------------ */
typedef enum {
    APP_STATE_MENU = 0,
    APP_STATE_PODOMETER
} AppState;

/* ------------------------------------------------------------------
   API publique
   ------------------------------------------------------------------ */
void Menu_Init(void);
void Menu_Draw(void);
AppState Menu_HandleTouch(void);

#endif /* MENU_H */

/**
 * @file    snake.h
 * @author  Candice
 * @brief   Module de gestion du jeu Snake pour l'écran ILI9341 sur STM32G4.
 *
 * @details Jeu non-bloquant conçu pour une super-boucle : appeler
 *          Snake_Update() puis Snake_Draw() toutes les Snake_GetDelay() ms.
 *
 * @section BoutonsSnake Boutons utilisés
 *  - PB4  : HAUT    (ButtonU)
 *  - PB6  : BAS     (ButtonD) — rejouer après Game Over
 *  - PB0  : GAUCHE  (ButtonL) — quitter après Game Over (géré dans main.c)
 *  - PA12 : DROITE  (ButtonR)
 */

#ifndef __SNAKE_H
#define __SNAKE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"

/**
 * @brief  Réinitialise la partie : serpent de 3 cases au centre, score zéro,
 *         direction droite, et premier fruit placé aléatoirement.
 * @note   L'écran TFT doit déjà être initialisé (fait par MENU_init).
 */
void     Snake_Init(void);

/**
 * @brief  Avance le jeu d'un pas : lit les boutons, déplace le serpent,
 *         gère les collisions (murs, corps) et la nourriture.
 * @note   À appeler toutes les Snake_GetDelay() millisecondes.
 */
void     Snake_Update(void);

/**
 * @brief  Dessine l'état courant (incrémental : tête, queue, fruit).
 *         Affiche l'écran Game Over avec le score si la partie est finie.
 */
void     Snake_Draw(void);

/**
 * @brief  Indique si la partie est terminée.
 * @retval 1 Game Over (collision mur ou corps).
 * @retval 0 Partie en cours.
 */
uint8_t  Snake_IsGameOver(void);

/**
 * @brief  Renvoie le délai entre deux pas de jeu, en ms.
 * @return 300 ms au départ, -2 ms par point, plancher à 150 ms.
 */
uint32_t Snake_GetDelay(void);

#ifdef __cplusplus
}
#endif

#endif /* __SNAKE_H */

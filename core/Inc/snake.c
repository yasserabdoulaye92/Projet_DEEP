/**
 * @file    snake.c
 * @author  Candice
 * @brief   Implémentation du jeu Snake de façon non-bloquante.
 *
 * @details Le serpent évolue sur une grille de 32x24 cases de 10 pixels.
 *          La vitesse augmente avec le score (délai 300 ms -> 150 ms).
 *          Le dessin est incrémental : seules la tête, la queue et la
 *          nourriture sont redessinées à chaque pas (pas de clignotement).
 *
 * @section BrochageSnake Boutons (croix directionnelle carte DEEP Purple)
 *  - ButtonU -> PB4  : HAUT
 *  - ButtonD -> PB6  : BAS (et "Rejouer" sur l'écran Game Over)
 *  - ButtonL -> PB0  : GAUCHE (et "Quitter" sur l'écran Game Over)
 *  - ButtonR -> PA12 : DROITE
 *  Boutons actifs à l'état bas (pull-up 10k + 100nF sur la carte).
 */

#include "snake.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "stm32g4xx_hal.h"   /* uint8_t, uint32_t, HAL_*, GPIO_* */
#include "stm32g4_flash.h"   /* sauvegarde du record en flash (module BSP) */
#include <stdlib.h>          /* rand() */

/* ── Constantes ── */
#define BLOCK_SIZE      10
#define GRID_WIDTH      32
#define GRID_HEIGHT     24
#define SNAKE_MAX_LEN   100
#define DEBOUNCE_MS     50

#define DELAY_INITIAL   300u
#define DELAY_MIN        150u
#define DELAY_STEP         2u

/* ── Types ── */
typedef enum  { UP, DOWN, LEFT, RIGHT } Direction;
typedef struct { int x; int y; }        Point;

/* ── Variables statiques ── */
static Point     snake[SNAKE_MAX_LEN];
static int       snake_length  = 3;
static Direction current_dir   = RIGHT;
static Point     food;
static int       game_over     = 0;
static Point     old_tail;
static uint8_t   has_eaten     = 0;
static uint32_t  last_press[4] = {0, 0, 0, 0};

/* ── Record sauvegardé en flash (complément F) ── */
#define CASE_FLASH_RECORD  0            /* case 0 de la page VIRTUAL_EEPROM (0x0801F800) */
#define MAGIC_RECORD       0x534E414BUL /* "SNAK" en ASCII : prouve qu'un record est écrit */

static uint32_t  record         = 0;    /* meilleur score connu (RAM, rechargé au boot) */
static uint8_t   nouveau_record = 0;    /* vrai si la partie qui vient de finir a battu le record */

/**
 * @brief  Relit le record stocké dans la dernière page de flash.
 * @return Le record, ou 0 si la flash est vierge (page effacée = 0xFF
 *         partout, donc le mot magique est absent).
 */
static uint32_t Lire_Record_Flash(void)
{
    uint64_t mot = BSP_FLASH_read_doubleword(CASE_FLASH_RECORD);
    if ((uint32_t)(mot & 0xFFFFFFFF) != MAGIC_RECORD)
        return 0;
    return (uint32_t)(mot >> 32);
}

/**
 * @brief  Termine la partie et sauvegarde le record en flash s'il est battu.
 * @note   L'écriture flash n'a lieu QUE sur un nouveau record (pas à chaque
 *         partie) : l'usure de la flash reste négligeable face aux
 *         ~10 000 cycles d'effacement garantis par ST.
 */
static void Fin_De_Partie(void)
{
    uint32_t score = (uint32_t)(snake_length - 3);
    game_over = 1;
    nouveau_record = 0;
    if (score > record) {
        record = score;
        BSP_FLASH_set_doubleword(CASE_FLASH_RECORD,
                                 ((uint64_t)record << 32) | MAGIC_RECORD);
        nouveau_record = 1;
    }
}

/* ── Fonctions privées ── */

/**
 * @brief Lit la croix directionnelle et change la direction du serpent.
 *        Le demi-tour direct (ex: gauche -> droite) est interdit.
 */
static void Read_Inputs(void)
{
    uint32_t now = HAL_GetTick();

    /* ButtonD (PB6) : BAS */
    if      (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)  == GPIO_PIN_RESET
             && current_dir != UP
             && (now - last_press[0]) > DEBOUNCE_MS)
    { current_dir = DOWN;  last_press[0] = now; }

    /* ButtonU (PB4) : HAUT */
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)  == GPIO_PIN_RESET
             && current_dir != DOWN
             && (now - last_press[1]) > DEBOUNCE_MS)
    { current_dir = UP;    last_press[1] = now; }

    /* ButtonR (PA12) : DROITE */
    else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET
             && current_dir != LEFT
             && (now - last_press[2]) > DEBOUNCE_MS)
    { current_dir = RIGHT; last_press[2] = now; }

    /* ButtonL (PB0) : GAUCHE */
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)  == GPIO_PIN_RESET
             && current_dir != RIGHT
             && (now - last_press[3]) > DEBOUNCE_MS)
    { current_dir = LEFT;  last_press[3] = now; }
}

static void Draw_Block(int grid_x, int grid_y, uint16_t color)
{
    uint16_t x0 = (uint16_t)(grid_x * BLOCK_SIZE);
    uint16_t y0 = (uint16_t)(grid_y * BLOCK_SIZE);
    ILI9341_DrawFilledRectangle(x0, y0,
                                x0 + BLOCK_SIZE - 1,
                                y0 + BLOCK_SIZE - 1,
                                color);
}

static void Spawn_Food(void)
{
    Point   candidate;
    uint8_t on_snake;

    do {
        on_snake    = 0;
        candidate.x = rand() % GRID_WIDTH;
        candidate.y = rand() % GRID_HEIGHT;

        for (int i = 0; i < snake_length; i++) {
            if (snake[i].x == candidate.x && snake[i].y == candidate.y) {
                on_snake = 1;
                break;
            }
        }
    } while (on_snake);

    food = candidate;
}

static uint32_t Get_Delay(void)
{
    int score = snake_length - 3;
    int delay = (int)DELAY_INITIAL - score * (int)DELAY_STEP;
    if (delay < (int)DELAY_MIN) delay = (int)DELAY_MIN;
    return (uint32_t)delay;
}

/* ── API publique ── */

void Snake_Init(void)
{
    /* On nettoie juste l'écran, le hardware TFT est déjà initialisé par le menu */
    ILI9341_Fill(ILI9341_COLOR_BLACK);

    snake[0].x = GRID_WIDTH  / 2;
    snake[0].y = GRID_HEIGHT / 2;
    snake[1].x = GRID_WIDTH  / 2 - 1;
    snake[1].y = GRID_HEIGHT / 2;
    snake[2].x = GRID_WIDTH  / 2 - 2;
    snake[2].y = GRID_HEIGHT / 2;

    snake_length  = 3;
    current_dir   = RIGHT;
    game_over     = 0;
    has_eaten     = 0;
    nouveau_record = 0;
    record        = Lire_Record_Flash(); /* relecture du record conservé en flash */
    last_press[0] = last_press[1] = last_press[2] = last_press[3] = 0;

    Spawn_Food();

    Draw_Block(food.x, food.y, ILI9341_COLOR_RED);
    for (int i = 0; i < snake_length; i++)
        Draw_Block(snake[i].x, snake[i].y, ILI9341_COLOR_GREEN);
}

void Snake_Update(void)
{
    if (game_over) {
        /* ButtonD (PB6) : rejouer */
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET) {
            HAL_Delay(DEBOUNCE_MS);
            Snake_Init();
        }
        return;
    }

    Read_Inputs();
    has_eaten = 0;
    old_tail  = snake[snake_length - 1];

    for (int i = snake_length - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    switch (current_dir) {
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }

    if (snake[0].x < 0 || snake[0].x >= GRID_WIDTH ||
        snake[0].y < 0 || snake[0].y >= GRID_HEIGHT)
    {
        Fin_De_Partie();
        return;
    }

    for (int i = 2; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            Fin_De_Partie();
            return;
        }
    }

    if (snake[0].x == food.x && snake[0].y == food.y) {
        has_eaten = 1;
        if (snake_length < SNAKE_MAX_LEN) {
            snake_length++;
            snake[snake_length - 1] = old_tail;
        }
        Spawn_Food();
    }
}

void Snake_Draw(void)
{
    if (game_over) {
        ILI9341_Fill(ILI9341_COLOR_BLACK);
        ILI9341_Puts(80, 100, "GAME OVER",
                     &Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK);
        ILI9341_printf(90, 125, &Font_7x10,
                       ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK,
                       "Score: %d", snake_length - 3);
        /* Record conservé en flash : survit à l'extinction de la carte */
        if (nouveau_record)
            ILI9341_printf(90, 140, &Font_7x10,
                           ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK,
                           "Record: %d  NOUVEAU !", (int)record);
        else
            ILI9341_printf(90, 140, &Font_7x10,
                           ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK,
                           "Record: %d", (int)record);
        ILI9341_Puts(30, 160, "DOWN: Rejouer | LEFT: Quitter",
                     &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
        return;
    }

    /* Efface l'ancienne queue seulement si le serpent n'a pas mangé */
    if (!has_eaten)
        Draw_Block(old_tail.x, old_tail.y, ILI9341_COLOR_BLACK);

    /* Dessine uniquement la nouvelle tête */
    Draw_Block(snake[0].x, snake[0].y, ILI9341_COLOR_GREEN);

    /* Redessine la nourriture si elle vient d'être régénérée */
    if (has_eaten) {
        Draw_Block(food.x, food.y, ILI9341_COLOR_RED);
    }
}

uint8_t Snake_IsGameOver(void)
{
    return (uint8_t)game_over;
}

uint32_t Snake_GetDelay(void)
{
    return Get_Delay();
}

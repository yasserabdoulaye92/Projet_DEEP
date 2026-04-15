/**
 *******************************************************************************
 * @file 	main.c
 * @author 	jjo
 * @date 	Mar 29, 2024
 * @brief	Fichier principal de votre projet sur carte Nucléo STM32G431KB
 *******************************************************************************
 */

#include "config.h"
#include "stm32g4_sys.h"
<<<<<<< Updated upstream

#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"

#include <stdio.h>

#define BLINK_DELAY		100	//ms

void write_LED(bool b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool char_received(uart_id_t uart_id)
{
	if( BSP_UART_data_ready(uart_id) )	/* Si un caractère est reçu sur l'UART 2*/
	{
		/* On "utilise" le caractère pour vider le buffer de réception */
		BSP_UART_get_next_byte(uart_id);
		return true;
	}
	else
		return false;
}

void heartbeat(void)
{
	while(! char_received(UART2_ID) )
	{
		write_LED(true);
		HAL_Delay(50);
		write_LED(false);
		HAL_Delay(1500);
	}
}


/**
  * @brief  Point d'entrée de votre application
  */
int main(void)
{
	/* Cette ligne doit rester la première de votre main !
	 * Elle permet d'initialiser toutes les couches basses des drivers (Hardware Abstraction Layer),
	 * condition préalable indispensable à l'exécution des lignes suivantes.
	 */
	HAL_Init();

	/* Initialisation des périphériques utilisés dans votre programme */
	BSP_GPIO_enable();
	BSP_UART_init(UART2_ID,115200);

	/* Indique que les printf sont dirigés vers l'UART2 */
	BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	/* Initialisation du port de la led Verte (carte Nucleo) */
	BSP_GPIO_pin_config(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH,GPIO_NO_AF);

	/* Hello student */
	printf("Hi <Student>, can you read me?\n");

	//heartbeat();

	/* Tâche de fond, boucle infinie, Infinite loop,... quelque soit son nom vous n'en sortirez jamais */
	while (1)
	{

		if( char_received(UART2_ID) )
		{
			write_LED(true);		/* write_LED? Faites un ctrl+clic dessus pour voir... */
			HAL_Delay(BLINK_DELAY);	/* ... ça fonctionne aussi avec les macros, les variables. C'est votre nouveau meilleur ami */
			write_LED(false);
		}

	}
=======
#include "NFC03A1/stm32g4_nfc03a1.h"
#include "NFC03A1/lib_nfc/lib_pcd/lib_PCD.h"
#include <stdio.h>

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    BSP_NFC03A1_Init(PCD);
    PCD_ProtocolSelect(0x02, PCDPROTOCOL_14443A, NULL, NULL);
    while (1)
    {
        uint8_t status = ConfigManager_TagHunting(TRACK_ALL);
        if (status == RFTRANS_95HF_SUCCESS_CODE)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
            HAL_Delay(2000);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
        }

        HAL_Delay(500);
    }
>>>>>>> Stashed changes
}

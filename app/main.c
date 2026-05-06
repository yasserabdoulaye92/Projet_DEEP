#include "config.h"
<<<<<<< Updated upstream
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"
#include <stdio.h>
#include "menu.h"

/* Prototypes des fonctions système (générées par l'IDE) */
void SystemClock_Config(void);

int main(void) {
HAL_Init();
    MENU_init();

    while (1) {
        // On surveille le tactile en permanence
        MENU_handler();


=======
#include "stm32g4_sys.h"
#include "stm32g4_uart.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>

void Bluetooth_Task(void);
void Sensors_Send_Task(void);

char buffer_reception[100];
int index_rec = 0;
uint32_t dernier_envoi_capteurs = 0;

int main(void) {
    HAL_Init();
    SystemClock_Config();

    MENU_init();
    BSP_UART_init(UART1_ID, 9600); // Initialisation du Bluetooth

    while (1) {
        MENU_handler();
        Bluetooth_Task();
        Sensors_Send_Task();
    }
}

// ==========================================
// TÂCHE BLUETOOTH (RÉCEPTION)
// ==========================================
void Bluetooth_Task(void) {
    if (BSP_UART_data_ready(UART1_ID)) {
        char lettre = BSP_UART_getc(UART1_ID);

        // Si on détecte la fin du message (touche Entrée depuis le téléphone)
        if (lettre == '\n' || lettre == '\r') {
            if (index_rec > 0) {
                buffer_reception[index_rec] = '\0'; // On ferme la phrase

                // Si le message commence par "N:"
                if (strncmp(buffer_reception, "N:", 2) == 0) {

                    // On envoie le texte situé APRÈS les deux premiers caractères ("N:")
                    // Exemple : "N:Coucou" -> On envoie "Coucou"
                    MENU_set_notif(&buffer_reception[2]);

                }

                index_rec = 0; // On remet le compteur à zéro pour le prochain message
            }
        } else if (index_rec < 99) {
            buffer_reception[index_rec++] = lettre;
        }
    }
}

// ==========================================
// TÂCHE D'ENVOI (VERS LE TÉLÉPHONE)
// ==========================================
void Sensors_Send_Task(void) {
    if (HAL_GetTick() - dernier_envoi_capteurs > 2000) {
        char buffer_tx[50];
        int bpm = 72; // Exemple fictif
        int temp = 37; // Exemple fictif

        sprintf(buffer_tx, "BPM:%d|Temp:%d C\n", bpm, temp);

        for (int i = 0; i < strlen(buffer_tx); i++) {
            BSP_UART_putc(UART1_ID, buffer_tx[i]);
        }

        dernier_envoi_capteurs = HAL_GetTick();
>>>>>>> Stashed changes
    }
}

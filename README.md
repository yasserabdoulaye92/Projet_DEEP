# Projet Smartwatch STM32 (DEEP)

## Description du projet

Le projet Smartwatch consiste en la conception et le développement d'une montre connectée dédiée au suivi de l'activité physique et des paramètres de santé. Le système repose sur une carte Nucleo STM32G431KB montée sur la carte d'extension DEEP Purple de l'ESEO. La montre communique avec un smartphone par Bluetooth pour recevoir les notifications, l'heure et la météo.

## Fonctionnalités

* **Horloge** : heure tenue par la RTC du STM32, réglable depuis le téléphone, avec un chronomètre.
* **Santé** : mesure du rythme cardiaque (capteur optique HW-827) et compteur de pas (accéléromètre MPU6050).
* **Notifications** : réception des messages du téléphone par Bluetooth, avec bip du buzzer.
* **Météo** : affichage des informations envoyées par le téléphone.
* **Jeu Snake** : jouable avec les 4 boutons de la carte. Le record est sauvegardé en mémoire flash et survit à l'extinction de la montre.
* **Réglages** : choix de la couleur du thème de l'interface.
* En cas de capteur débranché, la page podomètre affiche un diagnostic (état des lignes I2C et scan du bus) au lieu de planter.

## Matériel

| Périphérique | Liaison | Broches |
|---|---|---|
| Écran TFT ILI9341 + tactile XPT2046 | SPI | PA4–PA8, PB3, PB5, PA11 |
| Accéléromètre MPU6050 | I2C | PA15 (SCL), PB7 (SDA) |
| Capteur de pouls HW-827 | ADC | PA0 |
| Module Bluetooth HC-05 | UART (9600 bauds) | PA9 (TX), PA10 (RX) |
| Buzzer | GPIO + Timer 6 | PA1 |
| Boutons (haut, bas, gauche, droite) | GPIO | PB4, PB6, PB0, PA12 |

## Commandes Bluetooth

Les trames se terminent par un retour à la ligne :

* `N:message` : affiche une notification et fait sonner le buzzer
* `H:14:30` : règle l'heure de la montre
* `M:Soleil 25C` : met à jour la météo

## Organisation du dépôt

* `app/` : point d'entrée du programme (`main.c`) et configuration des modules (`config.h`)
* `core/Inc/` : modules applicatifs (menu, podomètre, buzzer, snake, capteur de pouls)
* `drivers/` : BSP fourni par l'ESEO et bibliothèque HAL de ST
* `docs/` : site de documentation généré par Doxygen
* `Doxyfile` : configuration Doxygen du projet

## Compilation

Le projet se compile avec STM32CubeIDE : importer le dossier comme projet existant, puis Build et Run sur la carte Nucleo branchée en USB.

## Documentation du code

Le code applicatif est commenté au format Doxygen. Pour régénérer le site de documentation, lancer `doxygen Doxyfile` à la racine du dépôt, puis ouvrir `docs/html/index.html`.

## Directives de contribution et Git workflow

1. **Branche principale :** la branche `main` est réservée aux versions stables et fonctionnelles.
2. **Feature branches :** chaque nouvelle tâche fait l'objet d'une branche dédiée à partir de `main` (ex. : `feature/capteur-bpm`).
3. **Modularité du code :** le fichier `main.c` reste aussi concis que possible, les développements sont encapsulés dans des modules dédiés (`.c`/`.h`).
4. **Intégration :** toute fusion vers `main` passe par une Pull Request relue par un membre de l'équipe.

## Conventions de code (Doxygen)

Chaque fichier et chaque fonction est commenté au format Doxygen :

```c
/**
 * @file    nom_du_fichier.c
 * @author  Prénom
 * @brief   Description courte du fichier.
 */

/**
 * @brief  Ce que fait la fonction.
 * @param  valeur Description du paramètre.
 * @return Ce que la fonction renvoie.
 */
```

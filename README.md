# Projet Smartwatch STM32

## Description du Projet
Le projet Smartwatch consiste en la conception et le développement d'une montre connectée dédiée au suivi de l'activité physique et des paramètres de santé. Reposant sur une architecture embarquée autour d'un microcontrôleur STM32, le système interagit avec une application mobile Android. Il offre une solution logicielle et matérielle de bout en bout : de l'acquisition des données physiologiques par les capteurs jusqu'à leur traitement, leur transmission sans fil et leur visualisation sur smartphone.

## Fonctionnalités Principales

### Système Embarqué (Montre)
* **Suivi physiologique en temps réel :** Acquisition des données de fréquence cardiaque, de la température corporelle et du nombre de pas (podomètre).
* **Interface utilisateur :** Affichage de l'heure via une horloge temps réel (RTC), des constantes physiologiques et gestion des notifications sur un écran tactile.
* **Connectivité :** Transmission des données sans fil via un module Bluetooth et système d'appairage/communication rapide via NFC.

### Application Mobile (Android)
* **Tableau de bord :** Affichage des mesures instantanées transmises par la montre.
* **Historique et analyse :** Enregistrement et suivi de l'évolution des données de santé dans le temps.
* **Paramétrage :** Interface de configuration et de contrôle de la smartwatch.

## Architecture et Technologies

* **Microcontrôleur :** Carte de développement STM32G431RB (Programmation en C via STM32CubeIDE).
* **Affichage :** Écran TFT intégrant un contrôleur tactile.
* **Périphériques matériels :** RTC, capteur de fréquence cardiaque, capteur de température, accéléromètre.
* **Développement Mobile :** Environnement Android Studio (Java/Kotlin).

## Organisation du Dépôt

Le dépôt est structuré de la manière suivante afin de séparer les différents environnements de développement :

* `Firmware_STM32/` : Projet STM32CubeIDE complet (code source embarqué, configuration `.ioc`, pilotes matériels). L'architecture logicielle y est modulaire.
* `App_Android/` : Code source de l'application mobile Android Studio.
* `Docs/` : Documentation technique, schémas de câblage et spécifications des composants (datasheets).

## Directives de Contribution et Git Workflow

Pour garantir la stabilité du projet et faciliter le travail en équipe, les règles suivantes s'appliquent :

1. **Branche principale :** La branche `main` est réservée aux versions stables et fonctionnelles. Aucun développement ne doit être effectué directement sur cette branche.
2. **Feature Branches :** L'équipe utilise le flux de travail par fonctionnalités. Chaque nouvelle tâche doit faire l'objet d'une branche isolée à partir de `main` (ex. : `feature/capteur-bpm`, `feature/interface-tactile`).
3. **Gestion du fichier `.ioc` :** Le fichier de configuration STM32CubeMX est critique. Toute modification (ajout d'une broche, activation d'un périphérique) nécessite d'en informer l'équipe au préalable pour éviter les conflits de fusion. Une fois le fichier mis à jour sur `main`, les autres membres doivent synchroniser leur branche locale.
4. **Modularité du code :** Le fichier `main.c` doit être maintenu aussi concis que possible. Les développements doivent être encapsulés dans des modules spécifiques (fichiers `.c` et `.h` dédiés).
5. **Intégration (Pull Requests) :** Toute fusion vers la branche `main` doit s'effectuer par le biais d'une Pull Request, après validation du code par un ou plusieurs membres de l'équipe.
##  Conventions de Code (Doxygen)
Pour que notre projet soit propre et que la documentation se génère automatiquement à la fin, nous utilisons le standard **Doxygen**. 
**Règle absolue :** Chaque nouveau fichier (`.c` ou `.h`) et chaque nouvelle fonction doit être commenté avec ce format exact.
### En-tête de fichier (à mettre tout en haut des .c et .h)
```c
/**
 * @file    nom_du_fichier.c
 * @author  [Ton Prénom]
 * @brief   Description courte de ce que fait ce fichier (ex: Gestion du capteur BPM).
 * @date    [Date de création]
 */

[English](README.md) | **Français**

# ⚡ Sonoff POW CT ESPHome : Débridé !

> **Migrez mon compteur d'énergie Sonoff POW CT vers ESPHome et libérez ses fonctionnalités avancées de suivi énergétique !**

[![ESPHome](https://img.shields.io/badge/ESPHome-2F7B9D?style=for-the-badge&logo=esphome&logoColor=white)](https://esphome.io/)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-41A8F5?style=for-the-badge&logo=home-assistant&logoColor=white)](https://www.home-assistant.io/)
[![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)
[![Project Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)](https://github.com/votre_profil/votre_repo/commits/main)
[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)]() 
---

## ✨ Fonctionnalités Clés

Ce firmware ESPHome sur mesure me permet d'exploiter pleinement le Sonoff POW CT, en offrant des fonctionnalités essentielles pour le monitoring énergétique, notamment dans les installations solaires :

* **Mesure Bidirectionnelle Avancée**
    * Lecture des valeurs de puissance et d'énergie **positives** (consommation) et **négatives** (production).
    * « Bidirectionnelle » signifie puissance/énergie signée sur le **seul CT**
      (import/export) ; cela n'ajoute pas un second CT. Le POW CT standard a un
      seul CT sur le canal A ; le canal B sert de référence de biais interne.
      Voir la section **Limites** ci-dessous.
    * Mises à jour rapides, avec une nouvelle valeur par seconde.

* **Compteurs d'Énergie Séparés**
    * Lecture directe de l'énergie accumulée, avec des capteurs d'énergie dédiés pour l'**importation** (consommation totale) et l'**exportation** (production totale).
    * Les énergies sont sauvegardées dans la mémoire flash.

* **Outils de Débogage Avancés**
    * Fonctions intégrées pour lire et écrire directement dans les registres du composant de mesure via les **Actions** de Home Assistant :
        * `my_sonoff_powct_read_register`
        * `my_sonoff_powct_write_register`

* **Calibrage de Précision**
    * La possibilité d'effectuer un calibrage fin des mesures pour garantir une **précision optimale**.

* **Intégration Native**
    * Intégration parfaite et locale avec Home Assistant via l'API native ESPHome (pas de Cloud).

## ⚠️ Limites

* **Un seul CT.** Le POW CT standard possède une seule pince CT sur le canal A.
  Le canal B sert de référence de biais interne, pas de seconde mesure. Mesurer
  deux circuits (réseau + PV/batterie) nécessite une seconde pince CT, un câblage
  sur le canal B, un calibrage dédié et la désactivation/refonte de la correction
  de biais : c'est une modification matérielle, pas une option YAML.
* **Variantes matérielles.** D'autres variantes, notamment le POW CT « Ring »
  100 A, ne sont pas garanties compatibles avec la même définition de carte.
  Voir le [Guide de flashage](Doc/FLASHING_FR.md).

## 🚀 Prochaines Étapes (Feuille de Route)

Je travaille activement à l'amélioration de cette configuration. Les fonctionnalités prévues pour les prochaines versions sont :

* **Plus de Flexibilité de Configuration**
    * Accès aux valeurs de calibrage.
    * Option pour choisir des mesures signées ou non-signées.
    * ... Dites-moi ce qui vous serait utile.
* **Ajout de Diagnostics**
    * Ajout de capteurs de diagnostic supplémentaires. Dites-moi ce que vous souhaitez.

## 📚 Documentation & Guides

Pour commencer l'installation et la configuration de mon appareil, veuillez consulter les guides détaillés :

| Guide | Description | Lien |
| :--- | :--- | :--- |
| **Guide d'Installation** | Procédure complète pour flasher et démarrer mon appareil. | **[Doc/INSTALL_FR.md](Doc/INSTALL\_FR.md)** |
| **Guide de Flashage** | Images factory vs app-only/OTA, offsets et récupération. | **[Doc/FLASHING_FR.md](Doc/FLASHING\_FR.md)** |
| **Guide de Calibrage** | Instructions pour effectuer un calibrage précis de mes mesures. | **[Doc/CALIBRATION_FR.md](Doc/CALIBRATION\_FR.md)** |
| **Guide de Debug des registres** | Une image vaut mieux qu'un long discourt... | **[Doc/DEBUG_FR.md](Doc/DEBUG_FR.md)** |
| **Détails de Configuration** | Explications détaillées sur les options spécifiques au Sonoff POW CT et l'utilisation des **fonctions de debug** avancées. | **[Doc/SONOFF_POWCT_CONFIG_FR.md](Doc/SONOFF\_POWCT\_CONFIG\_FR.md)** |
| **Datasheet** | Manuel technique du circuit de mesure d'énergie **CSE7761**. | **[Doc/CSE7761UserManual\_1672133056.pdf](Doc/CSE7761UserManual\_1672133056.pdf)** |
| **Documentation d'ingénierie** | Spécification, conception, plan de test et ADR (anglais). | **[Doc/README.md](Doc/README.md)** |

***

### 🤝 Contribuer

Les contributions, les rapports de bugs et les demandes de fonctionnalités sont les bienvenus ! N'hésitez pas à ouvrir une *issue* ou une *pull request*.

### [![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)

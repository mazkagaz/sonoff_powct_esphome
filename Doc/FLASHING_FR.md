[English](FLASHING.md) | **Français**

# Flasher le Sonoff POW CT — choisir la bonne image

Ce guide explique quel fichier flasher, pourquoi une image **app-only** peut
laisser l’appareil dans une boucle de démarrage, et comment récupérer. Il
s’applique à ESPHome et à Tasmota.

> **Sécurité d’abord.** Le POW CT est relié au secteur. Travaillez appareil
> débranché, ne connectez jamais le secteur et un adaptateur USB/série en même
> temps, et utilisez une alimentation 3,3 V correcte avec une masse commune. Si
> vous n’êtes pas à l’aise avec le câblage secteur, faites-vous aider par une
> personne qualifiée.

## 1. Les trois étages de démarrage

Un ESP32 ne démarre pas directement une application :

1. la **ROM** interne s’exécute d’abord et cherche une image de démarrage dans
   la flash ;
2. le **bootloader** de second étage (ESP-IDF ou Tasmota) prépare la flash et
   choisit l’application ;
3. l’**application** démarre et initialise le Wi-Fi, l’écran, le CSE7761, etc.

Si la ROM ne peut pas lire un bootloader valide, le log série affiche souvent
`invalid header: 0xffffffff` puis redémarre. Cela se produit **avant**
l’application : le CSE7761 et son étalonnage ne sont pas en cause.

## 2. Image factory vs image app-only/OTA

| | Image factory | Image app-only / OTA |
|---|---|---|
| Contenu | Bootloader + table de partitions + application | Application seule |
| Usage | Premier flash, récupération | Mise à jour OTA, ou écriture dans la partition app |
| Écrite à | `0x0` (image fusionnée ; l’outil gère les offsets) | Offset de la partition app, selon la table de partitions |
| Démarre seule ? | Oui | Seulement si le bootloader et la table sont déjà présents |

Une image factory est une **installation complète**. Une image app-only est un
**programme de remplacement** : elle a besoin du bootloader et de la table de
partitions existants pour pointer au bon endroit.

### ESPHome

- Le **Web Flasher ESPHome** et l’action **Install** du tableau de bord écrivent
  automatiquement une image factory complète (bootloader + table + application).
- Si vous utilisez **Manual download**, choisissez le format **factory/fusionné**
  si votre version ESPHome le propose (souvent `firmware.factory.bin`). Le fichier
  `firmware.bin` est l’image applicative et ne convient pas à un premier flash.
- Les noms exacts dépendent de la version ESPHome. En cas de doute, utilisez le
  Web Flasher/tableau de bord plutôt que de deviner.
- Ne flashez jamais une app-only `firmware.bin` à `0x0` ou `0x10000`.

### Tasmota

- `tasmota32.factory.bin` est l’**image d’installation complète** pour le premier
  flash.
- `tasmota32.bin` est l’**application** pour les mises à jour OTA (ou l’offset
  applicatif). Ce n’est pas une image de premier flash.
- Le Web Installer Tasmota choisit automatiquement l’image factory. Si vous avez
  flashé `tasmota32.bin` au début de la flash, effacez puis flashez à nouveau
  l’image factory.

## 3. Choisir le bon fichier

| Situation | Utiliser |
|---|---|
| Premier flash, appareil vierge ou inconnu | Image factory |
| Récupération après un flash raté | Image factory après `erase_flash` |
| Mise à jour normale d’un appareil qui fonctionne | Image OTA / app-only |
| Vous n’avez que `firmware.bin`/`tasmota32.bin` | Ne l’écrivez pas à `0x0` ; procurez-vous une image factory ou utilisez le Web Flasher |

**À propos des offsets :** le layout stock du POWCT testé place l’application à
`0x20000`, alors qu’un layout ESPHome 4 Mo par défaut place souvent app0 à
`0x10000`. Ces valeurs appartiennent à leurs tables de partitions respectives.
Une image factory écrit sa propre table ; une image app-only n’est valide qu’à
l’offset de la table actuellement présente sur l’appareil. Si vous ne connaissez
pas la table, utilisez une image factory.

## 4. Récupération manuelle avec esptool

Utilisez `esptool.py` (ou `esptool` dans les versions récentes). Remplacez
`<PORT>` par votre port série. Le POWCT standard est un ESP32 ; n’ajustez
`--chip` que si vos outils rapportent une autre puce.

### 4.1 Sauvegarder d’abord (recommandé)

`erase_flash` efface la NVS, y compris les données d’étalonnage d’usine. Si vous
pouvez vouloir revenir au firmware stock, sauvegardez toute la flash :

```bash
esptool.py --chip esp32 --port <PORT> read_flash 0 0x400000 backup.bin
```

Ajustez la taille si `flash_id` rapporte une flash différente.

### 4.2 Effacer

```bash
esptool.py --chip esp32 --port <PORT> erase_flash
```

### 4.3 Flasher une image factory

```bash
esptool.py --chip esp32 --port <PORT> --baud 460800 write_flash 0x0 <image-factory>.bin
```

- `0x0` est correct pour une image **factory** fusionnée.
- Si votre adaptateur est instable à 460800, omettez `--baud` ou utilisez
  `115200`.
- Si le stub esptool échoue, essayez `--no-stub` (les anciennes docs parlent de
  `-nostub` ; esptool moderne utilise `--no-stub`).
- Passez en mode bootloader en maintenant GPIO0 à la masse pendant le reset,
  puis relâchez après la connexion.

### 4.4 Après le flash

Laissez l’appareil démarrer et créer son point d’accès Wi-Fi. Utilisez les images
OTA/app-only pour les mises à jour suivantes ; ne reflashez pas l’app-only à
`0x0`.

## 5. Si `invalid header: 0xffffffff` persiste

La ROM ne lit pas de bootloader valide. Relevez :

- le fichier exact, son type (factory/app-only), la commande et l’offset ;
- le log série complet depuis le reset (pas seulement les lignes répétées) ;
- `esptool.py --chip esp32 --port <PORT> flash_id` (fabricant, taille, voltage) ;
- `espefuse.py --port <PORT> summary` en lecture seule (secure boot, flash
  encryption, VDD_SDIO) ;
- une photo recto/verso et la référence du flash ;
- le 3,3 V au démarrage (oscilloscope si possible) et l’alimentation utilisée.

Cela permet de distinguer un problème de mode/taille/voltage de flash, d’eFuses,
d’alimentation ou de variante matérielle.

## 6. Variantes matérielles

Le dépôt a été développé sur le Sonoff POW CT standard. D’autres variantes,
notamment le **POW CT « Ring » 100 A**, peuvent utiliser une carte, une flash ou
une configuration d’eFuses différente. Elles ne sont **pas garanties**
compatibles avec la même définition de carte. Si vous avez une telle variante,
fournissez les diagnostics ci-dessus (`flash_id`, `espefuse summary`, photos,
log) pour qu’elle puisse être documentée.

## 7. Checklist

- [ ] J’utilise une image **factory** pour le premier flash, pas une app-only.
- [ ] J’ai sauvegardé toute la flash si je peux avoir besoin de l’étalonnage stock.
- [ ] Je connais mon port série et le `--chip` rapporté par esptool.
- [ ] J’ai le log série complet si le boot échoue encore.
- [ ] Je peux fournir `flash_id`, `espefuse summary` et des photos de la carte.

## 8. Références officielles

- Web Flasher ESPHome : <https://web.esphome.io/>
- Guide de connexion physique ESPHome :
  <https://esphome.io/guides/physical_device_connection/>
- Démarrage Tasmota : <https://tasmota.github.io/docs/Getting-Started/>
- Flashage Tasmota : <https://tasmota.github.io/docs/Flashing/>
- Documentation esptool :
  <https://docs.espressif.com/projects/esptool/en/latest/esp32/>

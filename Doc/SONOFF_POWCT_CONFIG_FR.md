[English](SONOFF_POWCT_CONFIG.md) | **Français**

# Configuration du SONOFF POWCT

## Configuration GPIO

| Broche | Fonction |
|:-------|:---------|
| GPIO00 | Bouton poussoir (HIGH = off, LOW = on) |
| GPIO05 | TM1621 DA |
| GPIO13 | LED d’état (HIGH = off, LOW = on) |
| GPIO15 | LED Wi-Fi |
| GPIO17 | TM1621 CS |
| GPIO18 | TM1621 WR |
| GPIO21 | Relais 1 |
| GPIO23 | TM1621 RD |
| GPIO25 | CSE7761 Rx |
| GPIO26 | CSE7761 Tx |

## Mode d’étalonnage

- `calibration_mode: offset` (défaut) : correction de biais par le canal B ;
  conserve le comportement précédent.
- `calibration_mode: stock` : lit en lecture seule la calibration stock dans la
  NVS, applique les 18 coefficients quadratiques et les gammes basse/haute. Si
  aucun payload valide n’est trouvé, les coefficients identité sont utilisés et
  un avertissement est journalisé.

Le mode stock est opt-in ; sa précision de mesure n’est pas encore validée sur
matériel.

## Canaux de mesure et CT

| Canal | Usage dans ce firmware |
|:--|:--|
| A | Entrée CT ; le seul CT du POW CT standard |
| B | Référence de biais interne ; pas une seconde entrée CT |

Le CSE7761 possède deux canaux de courant, mais le POW CT standard n’expose
qu’un seul CT sur le canal A. Le canal B est utilisé par ce composant pour
estimer et corriger l’offset du canal A. Un vrai montage deux canaux nécessite
une seconde pince CT, un câblage sur le canal B, un calibrage dédié et la
désactivation/refonte de la correction de biais. Voir
[FLASHING_FR.md](FLASHING_FR.md) pour les variantes matérielles.

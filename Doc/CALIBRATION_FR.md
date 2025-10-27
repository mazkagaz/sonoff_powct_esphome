# Rapport d'Analyse de Corrélation et de Correction de Biais pour Compteur Bicanal

Ce rapport fournit une analyse quantitative de la corrélation entre les erreurs de mesure intrinsèques (biais) du Canal A et du Canal B, basée sur un ensemble de données étendu provenant du fichier journal fourni. L'objectif est d'établir une formule pour corriger les mesures du Canal A en utilisant l'erreur simultanée mesurée sur le Canal B inactif.

L'analyse confirme une **corrélation linéaire forte et stable** entre les biais du Courant ($I$) et de la Puissance ($P$). Les calculs utilisent toutes les mesures « à vide » disponibles (les valeurs réelles sont zéro).

Pour reproduire cela, on pourrait enregistrer des mesures « à vide » avec son propre Sonoff POWCT et ses propres conditions climatiques, puis adapter les valeurs.

---

## 1. Établissement de la Corrélation (Facteur d'Échelle $k$)

Étant donné que l'appareil est inactif, les valeurs mesurées sur les deux canaux ($X_{A, \text{mesuré}}$ et $X_{B, \text{mesuré}}$) représentent l'erreur ou le biais de mesure ($E$). La relation est modélisée par un facteur d'échelle constant $k$ : $E_A \approx k \cdot E_B$.

| Quantité | Biais Moyen Canal A ($\text{Moyenne}(E_A)$) | Biais Moyen Canal B ($\text{Moyenne}(E_B)$) | Facteur d'Échelle Moyen ($k$) |
| :---: | :---: | :---: | :---: |
| **Courant ($I$)** | $0.00723 \text{ A}$ | $0.00069 \text{ A}$ | $\mathbf{10.46}$ |
| **Puissance ($P$)** | $0.9123 \text{ W}$ | $-0.1025 \text{ W}$ | $\mathbf{-8.89}$ |

---

## 2. Analyse de Stabilité

L'**Écart Type (ET)** et le **Coefficient de Variation (CV)** ($\text{CV} = \frac{ET}{\text{Moyenne}}$) sont utilisés pour évaluer la stabilité du biais et de la corrélation sur l'ensemble des données.

### A. Stabilité du Biais par Canal

Le Coefficient de Variation est très faible sur toutes les métriques, indiquant que l'erreur de mesure de référence (biais) pour chaque canal est **très stable** dans le temps.

| Quantité | Canal | Écart Type (ET) | Coefficient de Variation (CV) |
| :---: | :---: | :---: | :---: |
| **Courant ($I$)** | A | $0.00006 \text{ A}$ | $0.83\%$ |
| | B | $0.00001 \text{ A}$ | $1.45\%$ |
| **Puissance ($P$)** | A | $0.0210 \text{ W}$ | $2.30\%$ |
| | B | $0.0035 \text{ W}$ | $3.41\%$ |

### B. Stabilité de la Corrélation (Ratio $k$)

La variation du facteur d'échelle $k$ est minime, confirmant que la relation linéaire utilisée pour la déduction est **hautement fiable**.

| Quantité | Ratio Moyen $k$ | CV du Ratio $k$ |
| :---: | :---: | :---: |
| **Courant ($I_A / I_B$)** | $10.46$ | $\mathbf{1.63\%}$ |
| **Puissance ($P_A / P_B$)** | $-8.89$ | $\mathbf{3.39\%}$ |

---

## 3. Formules de Déduction et de Correction Recommandées

Étant donné que le Canal B est maintenu inactif ($X_{B, \text{réel}}=0$), sa mesure $X_{B, \text{mesuré}}$ est traitée comme l'erreur de référence $E_B$. La valeur corrigée pour le Canal A ($X_{A, \text{corrigé}}$) est obtenue en soustrayant l'erreur déduite ($E_A$) de la valeur mesurée ($X_{A, \text{mesuré}}$) :

$$X_{A, \text{corrigé}} = X_{A, \text{mesuré}} - E_A$$

$$X_{A, \text{corrigé}} = X_{A, \text{mesuré}} - k \cdot X_{B, \text{mesuré}}$$

### Formule de Correction du Courant ($k_I \approx 10.46$)

$$\mathbf{I_{A, \text{corrigé}} \approx I_{A, \text{mesuré}} - 10.46 \cdot I_{B, \text{mesuré}}}$$

### Formule de Correction de la Puissance ($k_P \approx -8.89$)

$$\mathbf{P_{A, \text{corrigé}} \approx P_{A, \text{mesuré}} + 8.89 \cdot P_{B, \text{mesuré}}}$$

---

## Conclusion : Limites de la Méthode

Bien que la méthode de correction proposée soit étayée par une forte corrélation statistique, son efficacité est soumise aux limitations suivantes :

1.  **Hypothèse de Linéarité :** Le modèle suppose que la relation entre les deux erreurs reste strictement proportionnelle (linéaire) sur toute la plage de fonctionnement du courant et de la puissance (et pas seulement à zéro). Le facteur d'échelle $k$ pourrait changer lorsque le Canal A est soumis à une charge élevée.
2.  **Biais Constant du Canal B :** La méthode suppose que le biais sur le Canal B ($E_B$) est stable et n'est pas affecté par le courant circulant à travers le Canal A. En réalité, le couplage électrique ou thermique entre les deux canaux pourrait provoquer une dérive de $E_B$ lorsque $I_A$ est non nul.
3.  **Pas de Correction du Facteur d'Échelle :** Cette méthode ne corrige qu'une erreur de biais/décalage constante. Elle ne corrige pas les différences potentielles dans le *gain* (ou facteur d'échelle) des deux canaux, ce qui pourrait entraîner une augmentation des erreurs proportionnelles à mesure que la valeur mesurée augmente. Il s'agit principalement d'une **correction du biais au point zéro** utilisant le Canal B comme « capteur de référence d'erreur ».

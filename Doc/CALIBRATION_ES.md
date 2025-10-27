# Informe de Análisis de Correlación y Corrección de Sesgo para Medidor de Doble Canal

Este informe proporciona un análisis cuantitativo de la correlación entre los errores de medición inherentes (sesgo) del Canal A y del Canal B, basado en un conjunto de datos extendido del archivo de registro proporcionado. El objetivo es establecer una fórmula para corregir las mediciones del Canal A utilizando el error simultáneo medido en el Canal B inactivo.

El análisis confirma una **correlación lineal fuerte y estable** entre los sesgos de Corriente ($I$) y Potencia ($P$). Los cálculos utilizan todas las mediciones "inactivas" disponibles (los valores verdaderos son cero).

Para reproducir esto, se podrían registrar mediciones "inactivas" con un sonoff POWCT propio y sus propias condiciones climáticas, y luego adaptar los valores.

---

## 1. Establecimiento de la Correlación (Factor de Escala $k$)

Dado que el dispositivo está inactivo, los valores medidos en ambos canales ($X_{A, \text{medido}}$ y $X_{B, \text{medido}}$) representan el error o sesgo de medición ($E$). La relación se modela mediante un factor de escala constante $k$: $E_A \approx k \cdot E_B$.

| Cantidad | Sesgo Promedio Canal A ($\text{Media}(E_A)$) | Sesgo Promedio Canal B ($\text{Media}(E_B)$) | Factor de Escala Promedio ($k$) |
| :---: | :---: | :---: | :---: |
| **Corriente ($I$)** | $0.00723 \text{ A}$ | $0.00069 \text{ A}$ | $\mathbf{10.46}$ |
| **Potencia ($P$)** | $0.9123 \text{ W}$ | $-0.1025 \text{ W}$ | $\mathbf{-8.89}$ |

---

## 2. Análisis de Estabilidad

La **Desviación Estándar (DE)** y el **Coeficiente de Variación (CV)** ($\text{CV} = \frac{DE}{\text{Media}}$) se utilizan para evaluar la estabilidad del sesgo y la correlación a través del conjunto de datos.

### A. Estabilidad del Sesgo por Canal

El Coeficiente de Variación es muy bajo en todas las métricas, lo que indica que el error de medición de referencia (sesgo) para cada canal es **muy estable** a lo largo del tiempo.

| Cantidad | Canal | Desviación Estándar (DE) | Coeficiente de Variación (CV) |
| :---: | :---: | :---: | :---: |
| **Corriente ($I$)** | A | $0.00006 \text{ A}$ | $0.83\%$ |
| | B | $0.00001 \text{ A}$ | $1.45\%$ |
| **Potencia ($P$)** | A | $0.0210 \text{ W}$ | $2.30\%$ |
| | B | $0.0035 \text{ W}$ | $3.41\%$ |

### B. Estabilidad de la Correlación (Relación $k$)

La variación en el factor de escala $k$ es mínima, confirmando que la relación lineal utilizada para la deducción es **altamente confiable**.

| Cantidad | Relación Promedio $k$ | CV de la Relación $k$ |
| :---: | :---: | :---: |
| **Corriente ($I_A / I_B$)** | $10.46$ | $\mathbf{1.63\%}$ |
| **Potencia ($P_A / P_B$)** | $-8.89$ | $\mathbf{3.39\%}$ |

---

## 3. Fórmulas de Deducción y Corrección Recomendadas

Dado que el Canal B se mantiene inactivo ($X_{B, \text{verdadero}}=0$), su medición $X_{B, \text{medido}}$ se trata como el error de referencia $E_B$. El valor corregido para el Canal A ($X_{A, \text{corregido}}$) se obtiene restando el error deducido ($E_A$) del valor medido ($X_{A, \text{medido}}$):

$$X_{A, \text{corregido}} = X_{A, \text{medido}} - E_A$$

$$X_{A, \text{corregido}} = X_{A, \text{medido}} - k \cdot X_{B, \text{medido}}$$

### Fórmula de Corrección de Corriente ($k_I \approx 10.46$)

$$\mathbf{I_{A, \text{corregido}} \approx I_{A, \text{medido}} - 10.46 \cdot I_{B, \text{medido}}}$$

### Fórmula de Corrección de Potencia ($k_P \approx -8.89$)

$$\mathbf{P_{A, \text{corregido}} \approx P_{A, \text{medido}} + 8.89 \cdot P_{B, \text{medido}}}$$

---

## Conclusión: Limitaciones del Método

Si bien el método de corrección propuesto está respaldado por una fuerte correlación estadística, su efectividad está sujeta a las siguientes limitaciones:

1.  **Suposición de Linealidad:** El modelo asume que la relación entre los dos errores sigue siendo estrictamente proporcional (lineal) en todo el rango operativo de corriente y potencia (no solo en cero). El factor de escala $k$ puede cambiar cuando el Canal A está bajo alta carga.
2.  **Sesgo Constante del Canal B:** El método asume que el sesgo en el Canal B ($E_B$) es estable y no se ve afectado por la corriente que fluye a través del Canal A. En realidad, el acoplamiento eléctrico o térmico entre los dos canales podría hacer que $E_B$ varíe cuando $I_A$ no es cero.
3.  **Sin Corrección del Factor de Escala:** Este método solo corrige un error de sesgo/desplazamiento constante. No corrige las posibles diferencias en la *ganancia* (o factor de escala) de los dos canales, lo que podría conducir a un aumento de errores proporcionales a medida que el valor medido se incrementa. Esta es principalmente una **corrección de sesgo de punto cero** utilizando el Canal B como un "sensor de referencia de error".

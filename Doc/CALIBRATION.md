# Correlation Analysis and Bias Correction Report for Dual-Channel Meter

This report provides a quantitative analysis of the correlation between the inherent measurement errors (bias) of Channel A and Channel B, based on an extended dataset from the provided log file. The goal is to establish a formula to correct Channel A measurements using the simultaneous error measured on the idle Channel B.

The analysis confirms a **strong and stable linear correlation** between the biases of Current ($I$) and Power ($P$). The calculations use all available "idle" measurements (true values are zero).

To reproduce this, one could logs "idle" measurements with its own sonoff POWCT and its own climatic conditions, and then adpats the values.

---

## 1. Establishing the Correlation (Scale Factor $k$)

Since the device is idle, the measured values on both channels ($X_{A, \text{measured}}$ and $X_{B, \text{measured}}$) represent the measurement error or bias ($E$). The relationship is modeled by a constant scale factor $k$: $E_A \approx k \cdot E_B$.

| Quantity | Average Bias Channel A ($\text{Mean}(E_A)$) | Average Bias Channel B ($\text{Mean}(E_B)$) | Average Scale Factor ($k$) |
| :---: | :---: | :---: | :---: |
| **Current ($I$)** | $0.00723 \text{ A}$ | $0.00069 \text{ A}$ | $\mathbf{10.46}$ |
| **Power ($P$)** | $0.9123 \text{ W}$ | $-0.1025 \text{ W}$ | $\mathbf{-8.89}$ |

---

## 2. Stability Analysis

The **Standard Deviation (SD)** and the **Coefficient of Variation (CV)** ($\text{CV} = \frac{SD}{\text{Mean}}$) are used to assess the stability of the bias and the correlation across the dataset.

### A. Bias Stability per Channel

The Coefficient of Variation is very low across all metrics, indicating that the baseline measurement error (bias) for each channel is **very stable** over time.

| Quantity | Channel | Standard Deviation (SD) | Coefficient of Variation (CV) |
| :---: | :---: | :---: | :---: |
| **Current ($I$)** | A | $0.00006 \text{ A}$ | $0.83\%$ |
| | B | $0.00001 \text{ A}$ | $1.45\%$ |
| **Power ($P$)** | A | $0.0210 \text{ W}$ | $2.30\%$ |
| | B | $0.0035 \text{ W}$ | $3.41\%$ |

### B. Stability of Correlation (Ratio $k$)

The variation in the scale factor $k$ is minimal, confirming that the linear relationship used for deduction is **highly reliable**.

| Quantity | Average Ratio $k$ | CV of Ratio $k$ |
| :---: | :---: | :---: |
| **Current ($I_A / I_B$)** | $10.46$ | $\mathbf{1.63\%}$ |
| **Power ($P_A / P_B$)** | $-8.89$ | $\mathbf{3.39\%}$ |

---

## 3. Recommended Deduction and Correction Formulas

Since Channel B is kept idle ($X_{B, \text{true}}=0$), its measurement $X_{B, \text{measured}}$ is treated as the reference error $E_B$. The corrected value for Channel A ($X_{A, \text{corrected}}$) is obtained by subtracting the deduced error ($E_A$) from the measured value ($X_{A, \text{measured}}$):

$$X_{A, \text{corrected}} = X_{A, \text{measured}} - E_A$$

$$X_{A, \text{corrected}} = X_{A, \text{measured}} - k \cdot X_{B, \text{measured}}$$

### Current Correction Formula ($k_I \approx 10.46$)

$$\mathbf{I_{A, \text{corrected}} \approx I_{A, \text{measured}} - 10.46 \cdot I_{B, \text{measured}}}$$

### Power Correction Formula ($k_P \approx -8.89$)

$$\mathbf{P_{A, \text{corrected}} \approx P_{A, \text{measured}} + 8.89 \cdot P_{B, \text{measured}}}$$

---

## Conclusion: Limitations of the Method

While the proposed correction method is supported by a strong statistical correlation, its effectiveness is subject to the following limitations:

1.  **Linearity Assumption:** The model assumes the relationship between the two errors remains strictly proportional (linear) across the full operational range of current and power (not just at zero). The scale factor $k$ may change when Channel A is under high load.
2.  **Constant B Channel Bias:** The method assumes that the bias on Channel B ($E_B$) is stable and unaffected by current flowing through Channel A. In reality, electrical or thermal coupling between the two channels might cause $E_B$ to drift when $I_A$ is non-zero.
3.  **No Scale Factor Correction:** This method only corrects for a constant bias/offset error. It does not correct for potential differences in the *gain* (or scale factor) of the two channels, which could lead to increasing proportional errors as the measured value increases. This is primarily a **zero-point bias correction** using Channel B as an "error reference sensor."

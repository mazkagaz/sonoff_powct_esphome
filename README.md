**English** | [Français](README_FR.md)

# ⚡ Sonoff POW CT ESPHome: Unleashed!

> **Migrate my Sonoff POW CT energy meter to ESPHome and unlock its advanced energy monitoring features!**

[![ESPHome](https://img.shields.io/badge/ESPHome-2F7B9D?style=for-the-badge&logo=esphome&logoColor=white)](https://esphome.io/)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-41A8F5?style=for-the-badge&logo=home-assistant&logoColor=white)](https://www.home-assistant.io/)
[![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)
[![Project Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)](https://github.com/votre_profil/votre_repo/commits/main)
[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)]() 
---

## ✨ Key Features

This custom ESPHome firmware allows me to fully utilize the Sonoff POW CT, providing essential energy monitoring features, especially for solar installations:

* **Advanced Bidirectional Measurement**
    * Reading of **positive** (consumption) and **negative** (production) power and energy values.
    * “Bidirectional” means signed power/energy on the **single CT** (import/export);
      it does not add a second CT. The standard POW CT has one CT on channel A;
      channel B is used internally as a bias reference. See the **Limitations**
      section below.
    * Fast updates, with a new value per second.

* **Separate Energy Meters**
    * Direct reading of accumulated energy, with dedicated energy sensors for **import** (total consumption) and **export** (total production).
    * Energies are backed up to the flash memory.

* **Advanced Debugging Tools**
    * Built-in functions to directly read and write to the measurement component registers via Home Assistant **Actions**:
        * `my_sonoff_powct_read_register`
        * `my_sonoff_powct_write_register`

* **Precision Calibration**
    * The ability to perform fine calibration of measurements to ensure **optimal accuracy**.

* **Native Integration**
    * Perfect and local integration with Home Assistant via the native ESPHome API (No Cloud).

## ⚠️ Limitations

* **Single CT.** The standard POW CT has one CT clamp on channel A. Channel B is
  used internally as a bias reference, not as a second measurement. Measuring two
  circuits (grid + PV/battery) requires a second CT, wiring to channel B, a
  dedicated calibration, and disabling/rethinking the bias correction: that is a
  hardware modification, not a YAML option.
* **Hardware variants.** Other variants, including the POW CT “Ring” 100 A, are
  not guaranteed to work with the same board definition. See the
  [Flashing Guide](Doc/FLASHING.md).

## 🚀 Next Steps (Roadmap)

I am actively working on improving this configuration.

* **More configuration flexibility**
    * Calibration values access.
    * Switch to choose signed or unsigned measurements.
    * ... Tell me what would be useful.
* **Additional Diagnostics**
    * Addition of extra diagnostic sensors. Tell me what you wish.

## 📚 Documentation & Guides

To begin the installation and configuration of my device, please consult the detailed guides:

| Guide | Description | Link |
| :--- | :--- | :--- |
| **Installation Guide** | Complete procedure for flashing and starting my device. | **[Doc/INSTALL.md](Doc/INSTALL.md)** |
| **Flashing Guide** | Factory vs app-only/OTA images, offsets and recovery. | **[Doc/FLASHING.md](Doc/FLASHING.md)** |
| **Calibration Guide** | Instructions for performing precise calibration of my measurements. | **[Doc/CALIBRATION.md](Doc/CALIBRATION.md)** |
| **Register Debug Guide** | A picture is worth a thousand words... | **[Doc/DEBUG.md](Doc/DEBUG.md)** |
| **Configuration Details** | Detailed explanations of the Sonoff POW CT specific options and the use of advanced **debug functions**. | **[Doc/SONOFF\_POWCT\_CONFIG.md](Doc/SONOFF\_POWCT\_CONFIG.md)** |
| **Datasheet** | Technical manual for the **CSE7761** energy measurement circuit. | **[Doc/CSE7761UserManual\_1672133056.pdf](Doc/CSE7761UserManual\_1672133056.pdf)** |
| **Engineering Docs** | Specification, design, test plan and ADRs (English). | **[Doc/README.md](Doc/README.md)** |
| **Validated Versions** | ESPHome, ESP-IDF and development tool versions. | **[Doc/VERSIONS.md](Doc/VERSIONS.md)** |

***

### 🤝 Contributing

Contributions, bug reports, and feature requests are welcome! Feel free to open an *issue* or a *pull request*.

### [![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)

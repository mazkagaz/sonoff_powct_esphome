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
    * Reading of **positive** (consumption) and **negative** (production) current and power values.
    * Fast updates, with a new value per second.

* **Advanced Debugging Tools**
    * Built-in functions to directly read and write to the measurement component registers via Home Assistant **Actions**:
        * `my_sonoff_powct_read_register`
        * `my_sonoff_powct_write_register`

* **Precision Calibration**
    * The ability to perform fine calibration of measurements to ensure **optimal accuracy**.

* **Native Integration**
    * Perfect and local integration with Home Assistant via the native ESPHome API (No Cloud).

## 🚀 Next Steps (Roadmap)

I am actively working on improving this configuration. Planned features for upcoming versions include:

* **Separate Energy Meters**
    * Direct reading of accumulated energy, with dedicated energy sensors for **import** (total consumption) and **export** (total production).
* **Additional Diagnostics**
    * Addition of extra diagnostic sensors (Voltage, Power Factor, etc.).

## 📚 Documentation & Guides

To begin the installation and configuration of my device, please consult the detailed guides:

| Guide | Description | Link |
| :--- | :--- | :--- |
| **Installation Guide** | Complete procedure for flashing and starting my device. | **[Doc/INSTALL.md](Doc/INSTALL.md)** |
| **Calibration Guide** | Instructions for performing precise calibration of my measurements. | **[Doc/CALIBRATION.md](Doc/CALIBRATION.md)** |
| **Register Debug Guide** | A picture is worth a thousand words... | **[Doc/DEBUG.md](Doc/DEBUG.md)** |
| **Configuration Details** | Detailed explanations of the Sonoff POW CT specific options and the use of advanced **debug functions**. | **[Doc/SONOFF\_POWCT\_CONFIG.md](Doc/SONOFF\_POWCT\_CONFIG.md)** |
| **Datasheet** | Technical manual for the **CSE7761** energy measurement circuit. | **[Doc/CSE7761UserManual\_1672133056.pdf](Doc/CSE7761UserManual\_1672133056.pdf)** |

***

### 🤝 Contributing

Contributions, bug reports, and feature requests are welcome! Feel free to open an *issue* or a *pull request*.

### [![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)

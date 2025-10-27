# ⚡ Sonoff POW CT ESPHome: ¡Desencadenado!

> **¡Migra mi medidor de energía Sonoff POW CT a ESPHome y libera sus funciones avanzadas de monitorización energética!**

[![ESPHome](https://img.shields.io/badge/ESPHome-2F7B9D?style=for-the-badge&logo=esphome&logoColor=white)](https://esphome.io/)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-41A8F5?style=for-the-badge&logo=home-assistant&logoColor=white)](https://www.home-assistant.io/)
[![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)
[![Project Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)](https://github.com/votre_profil/votre_repo/commits/main)
[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)]() 
---

## ✨ Características Clave

Este firmware ESPHome personalizado me permite utilizar completamente el Sonoff POW CT, ofreciendo funciones esenciales para la monitorización energética, especialmente en instalaciones solares:

* **Medición Bidireccional Avanzada**
    * Lectura de valores de corriente y potencia **positivos** (consumo) y **negativos** (producción).
    * Actualizaciones rápidas, con un nuevo valor por segundo.

* **Herramientas de Depuración Avanzadas**
    * Funciones integradas para leer y escribir directamente en los registros del componente de medición a través de las **Acciones** de Home Assistant:
        * `my_sonoff_powct_read_register`
        * `my_sonoff_powct_write_register`

* **Calibración de Precisión**
    * La posibilidad de realizar una calibración fina de las mediciones para garantizar una **precisión óptima**.

* **Integración Nativa**
    * Integración perfecta y local con Home Assistant a través de la API nativa de ESPHome (sin Nube).

## 🚀 Próximos Pasos (Hoja de Ruta)

Estoy trabajando activamente para mejorar esta configuración. Las funciones previstas para las próximas versiones son:

* **Contadores de Energía Separados**
    * Lectura directa de la energía acumulada, con sensores de energía dedicados para la **importación** (consumo total) y la **exportación** (producción total).
* **Adición de Diagnósticos**
    * Adición de sensores de diagnóstico adicionales (Voltaje, Factor de Potencia, etc.).

## 📚 Documentación y Guías

Para comenzar la instalación y configuración de mi dispositivo, consulte las guías detalladas:

| Guía | Descripción | Enlace |
| :--- | :--- | :--- |
| **Guía de Instalación** | Procedimiento completo para flashear e iniciar mi dispositivo. | **[Doc/INSTALL.md](Doc/INSTALL\_ES.md)** |
| **Guía de Calibración** | Instrucciones para realizar una calibración precisa de mis mediciones. | **[Doc/CALIBRATION.md](Doc/CALIBRATION\_ES.md)** |
| **Guía de Depuración de Registros** | Una imagen vale más que mil palabras... | **[Doc/DEBUG.md](Doc/DEBUG.md)** |
| **Detalles de Configuración** | Explicaciones detalladas sobre las opciones específicas del Sonoff POW CT y el uso de las funciones de **depuración** avanzadas. | **[Doc/SONOFF\_POWCT\_CONFIG.md](Doc/SONOFF\_POWCT\_CONFIG.md)** |
| **Hoja de Datos** | Manual técnico del circuito de medición de energía **CSE7761**. | **[Doc/CSE7761UserManual\_1672133056.pdf](Doc/CSE7761UserManual\_1672133056.pdf)** |

***

### 🤝 Contribuir

¡Las contribuciones, los informes de errores y las solicitudes de funciones son bienvenidos! No dude en abrir una *issue* o una *pull request*.

**Nota:** Por favor, asegúrese de que todas las solicitudes (issues/pull requests) estén escritas en **francés** o en **inglés**.

### [![Buy me a coffee](https://img.shields.io/badge/Buy%20me%20a%20coffee-PayPal-blue?style=for-the-badge&logo=paypal)](https://paypal.me/mazkagaz)

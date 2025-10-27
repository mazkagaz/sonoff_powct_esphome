***

## 🛠️ Procedimiento Inicial de Flasheo

Para realizar el **flasheo inicial del firmware** en su dispositivo, deberá seguir algunos pasos de preparación del hardware.

---

### **Paso 1: Preparación del Hardware (🚨 ¡Apague la Alimentación!)**

Primero, debe **abrir la carcasa del dispositivo** mientras está **APAGADO** (desenchufado o desconectado del voltaje de la red) para garantizar la seguridad.

Dentro del dispositivo, debe **soldar cuatro cables conectores** a los puntos de programación claramente marcados en la placa: **3.3V, TX, RX y GND**. Estas son las conexiones estándar requeridas para flashear dispositivos basados en ESP.

**_Ejemplo de puntos de soldadura en la placa Sonoff POW CT:_**
![Pinout Sonoff POW CT](https://raw.githubusercontent.com/mazkagaz/sonoff_powct_esphome/Images/PCB.jpg)

| Pin | Función |
| :--- | :--- |
| **3.3V** | Suministro de Energía |
| **TX** | Transmitir Datos |
| **RX** | Recibir Datos |
| **GND** | Tierra (Ground) |

---

### **Paso 2: Conexión a su PC**

A continuación, necesitará un **Adaptador Serial USB-a-TTL** para establecer el puente de conexión entre el dispositivo y su computadora. Un ejemplo común y confiable es un módulo basado en el chip **CP2102**.

1.  Conecte los cables que acaba de soldar en el dispositivo a los pines correspondientes del adaptador USB-a-TTL:

    * Dispositivo **3.3V** $\rightarrow$ Adaptador **3.3V**
    * Dispositivo **TX** $\rightarrow$ Adaptador **RX**
    * Dispositivo **RX** $\rightarrow$ Adaptador **TX**
    * Dispositivo **GND** $\rightarrow$ Adaptador **GND**

2.  Enchufe el adaptador USB-a-TTL en un **puerto USB** libre de su PC.

💡 **Consejo Profesional:** Asegúrese de cruzar las conexiones TX/RX (**TX** a **RX** y **RX** a **TX**).

---

### **Paso 3: Flasheo del Firmware (ESPHome)**

Una vez que el hardware esté conectado, puede proceder con la carga del firmware utilizando la plataforma ESPHome.

1.  Coloque el dispositivo en **Modo de Flasheo/Bootloader** (esto generalmente implica presionar y mantener presionado un botón mientras conecta el GND).
2.  Utilice el **método de flasheo estándar de ESPHome** (p. ej., el Dashboard de ESPHome, la línea de comandos o el Web Flasher) para cargar el firmware.
3.  Utilice las **plantillas de configuración YAML** proporcionadas en la raíz de este repositorio.

🔗 **Video Tutorial:** Si es nuevo en este proceso, esta guía podría serle útil: [How to Install ESPHome on Sonoff Basic and Home Assistant](http://www.youtube.com/watch?v=4Q3whVVVwYw) por Reid Projects.

***

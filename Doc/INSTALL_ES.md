***

## 🛠️ Procedimiento Inicial de Flasheo

Para realizar el **flasheo inicial del firmware** en su dispositivo, deberá seguir algunos pasos de preparación del hardware.

---

### **Paso 1: Preparación del Hardware (🚨 ¡Apague la Alimentación!)**

Primero, debe **abrir la carcasa del dispositivo** mientras está **APAGADO** (desenchufado o desconectado del voltaje de la red) para garantizar la seguridad.

Dentro del dispositivo, debe **soldar cuatro cables conectores** a los puntos de programación claramente marcados en la placa: **3.3V, TX, RX y GND**. Estas son las conexiones estándar requeridas para flashear dispositivos basados en ESP.

**_Ejemplo de puntos de soldadura en la placa Sonoff POW CT:_**
[https://private-user-images.githubusercontent.com/10054386/344339282-9b7e1b35-457b-4beb-9941-75907833106e.jpg?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NjE1MzYyNjQsIm5iZiI6MTc2MTUzNTk2NCwicGF0aCI6Ii8xMDA1NDM4Ni8zNDQzMzkyODItOWI3ZTFiMzUtNDU3Yi00YmViLTk5NDEtNzU5MDc4MzMxMDZlLmpwZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNTEwMjclMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjUxMDI3VDAzMzI0NFomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPWU2ZDBlYWU3N2IzYTllYmNmZTJhOWUyNDRiOTYwZmEwNTIzMzE4MGI5MWFiY2NhNzBkYWEzMDY1N2QyMGQ3OTUmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0In0.-4w9kQ44bjtfMzJtfNrFyaouKQrcQGQM6Voq2nnQioM]

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

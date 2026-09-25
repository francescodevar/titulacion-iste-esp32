# ESP32 Wi‑Fi IDS

Prototipo académico de un sistema básico de detección de intrusiones Wi‑Fi con ESP32.

## Funcionalidad

- Monitorea tramas Wi‑Fi en modo promiscuo.
- Cuenta tramas de desautenticación (`0xC0`).
- Activa una alarma al superar un umbral.
- Muestra el estado en una pantalla OLED SSD1306.
- Utiliza LED verde para estado normal y LED rojo para alerta.
- Activa un buzzer durante una alerta.
- El botón permite:
  - simular una alerta en modo demostración;
  - silenciar una alerta activa.

## Hardware

- ESP32
- OLED I2C SSD1306
- LED verde
- LED rojo
- Buzzer
- Pulsador
- Resistencias y protoboard

## Conexiones usadas por el código

| Componente | Pin ESP32 |
|---|---:|
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| LED verde | GPIO 2 |
| LED rojo | GPIO 4 |
| Buzzer | GPIO 15 |
| Botón | GPIO 16 |

El botón utiliza `INPUT_PULLUP`.

## Librerías

Instalar desde el Library Manager de Arduino IDE:

- Adafruit GFX Library
- Adafruit SSD1306
- Adafruit BusIO

El soporte ESP32 debe estar instalado en Arduino IDE.

## Uso

1. Abrir `esp32-wifi-ids.ino` en Arduino IDE.
2. Seleccionar la placa ESP32 correspondiente.
3. Compilar y subir el programa.
4. Abrir el Monitor Serie a `115200`.
5. En estado normal, la OLED muestra `Monitoreando... / Red segura`.
6. Presionar el botón para activar el modo demostración.
7. Presionarlo de nuevo durante la alerta para silenciarla.

## Nota sobre la pantalla OLED

El código final del proyecto usa:

- resolución `128x64`;
- dirección I2C `0x3C`.

Si el módulo físico utiliza otra resolución o dirección, estos valores deben ajustarse antes de cargar el programa. Durante las pruebas se consideró `0x3D` como dirección alternativa.

## Alcance académico

El sistema es un prototipo educativo. Una alerta indica que se alcanzó el umbral configurado de tramas de desautenticación observadas; no constituye por sí sola una confirmación forense de una intrusión.

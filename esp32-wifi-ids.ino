/*
 * PROYECTO: IDS WiFi Doméstico con ESP32
 * Descripción: Sistema de Detección de Intrusos que monitorea el aire en busca de
 * ataques de desautenticación (Deauth) y alerta mediante LED, zumbador y pantalla OLED.
 *
 * Hardware: ESP32, Pantalla OLED I2C (SSD1306), LED Verde, LED Rojo, Zumbador Activo, Botón.
 * Pines:
 *   - OLED: SDA=21, SCL=22, VCC=3V3, GND=GND
 *   - LED Verde=2, LED Rojo=4, Zumbador=15, Botón=16 (con INPUT_PULLUP)
 *
 * Nota: Si la pantalla OLED no enciende, cambiar la dirección 0x3C por 0x3D.
 */

#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuración de la Pantalla OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Definición de Pines ---
#define LED_VERDE 2
#define LED_ROJO  4
#define BUZZER    15
#define BOTON     16

// --- Variables para la Detección ---
volatile int deauthCount = 0;
unsigned long lastDeauthTime = 0;
const int DEAUTH_THRESHOLD = 20; // Umbral de paquetes Deauth para activar alarma
bool alarmaActiva = false;

// --- Variables para el Antirrebote del Botón (Debounce) ---
unsigned long ultimoTiempoBoton = 0;
const unsigned long intervaloDebounce = 300; // 300ms para evitar rebotes mecánicos

// --- Callback del Modo Promiscuo (El "oído" del IDS) ---
// Esta función se ejecuta cada vez que el ESP32 captura un paquete WiFi.
void wifiSniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  uint8_t *payload = pkt->payload;

  // El byte 0xC0 en el payload indica una trama de desautenticación (Deauth Attack)
  if (payload[0] == 0xC0) {
    deauthCount++;
    lastDeauthTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configuración de pines
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTON, INPUT_PULLUP); // Resistencia pull-up interna

  // Inicialización de la pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error: No se pudo inicializar la pantalla OLED");
    for(;;); // Bucle infinito si falla la pantalla
  }

  // Mensaje de bienvenida
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Iniciando IDS WiFi...");
  display.display();

  // Configuración del WiFi en modo promiscuo
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifiSniffer);

  Serial.println("IDS WiFi iniciado. Monitoreando...");
  Serial.println("Presiona el boton para SIMULAR un ataque (Modo Demo).");
  actualizarSalidas();
}

void loop() {
  // 1. Detección de ataques reales (si el contador supera el umbral)
  if (deauthCount >= DEAUTH_THRESHOLD) {
    alarmaActiva = true;
    deauthCount = 0;
    Serial.println("¡ALERTA! Ataque deautenticación detectado (REAL).");
  }

  // 2. Apagado automático de la alarma tras 10 segundos sin ataques
  if (alarmaActiva && (millis() - lastDeauthTime > 10000)) {
    alarmaActiva = false;
    Serial.println("Alarma apagada automáticamente (tiempo de espera).");
  }

  // 3. Lectura del Botón (con antirrebote y modo simulación para demo)
  if (digitalRead(BOTON) == LOW) {
    unsigned long tiempoActual = millis();
    if (tiempoActual - ultimoTiempoBoton > intervaloDebounce) {

      if (alarmaActiva) {
        // Si la alarma está sonando, el botón la silencia
        Serial.println("Botón presionado. Silenciando alarma...");
        alarmaActiva = false;
        deauthCount = 0;
        digitalWrite(BUZZER, LOW);
      } else {
        // MODO DEMO: Si no hay alarma, el botón simula un ataque para probar el sistema
        Serial.println("¡MODO DEMO! Simulando ataque para prueba de pantalla y zumbador.");
        alarmaActiva = true;
        lastDeauthTime = millis();
      }
      ultimoTiempoBoton = tiempoActual;
    }
  }

  // 4. Actualizar salidas (LEDs, Zumbador, Pantalla)
  actualizarSalidas();
  delay(50);
}

// --- Función para actualizar el estado visual y sonoro ---
void actualizarSalidas() {
  if (alarmaActiva) {
    // ESTADO DE ALERTA
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(BUZZER, HIGH);

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("¡ALERTA!");
    display.println("Ataque deauth");
    display.println("detectado");
    display.display();
  } else {
    // ESTADO NORMAL (Monitoreando)
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(BUZZER, LOW);

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Monitoreando...");
    display.println("Red segura");
    display.display();
  }
}

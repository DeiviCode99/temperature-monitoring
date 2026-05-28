/*
 * Estaci�n Meteorol�gica IoT
 * ESP32 + DHT11 + BMP180 + FC-28 + LCD1602 I2C
 * Env�a datos a Django REST API cada 30 segundos
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <LiquidCrystal_I2C.h>

// ============ CONFIGURACI�N WIFI ============
const char* SSID = "ESP32TEST";
const char* PASSWORD = "12345678";

// ============ CONFIGURACI�N API ============
const char* API_URL = "http://192.168.1.100:8000/api/lecturas/";
const char* API_KEY = "ESP32_API_KEY_2024";
const char* DEVICE_ID = "ESP32_01";

// ============ PINES ============
#define DHTPIN 4
#define DHTTYPE DHT11
#define FC28_PIN 34
#define BMP_SDA 21
#define BMP_SCL 22

// ============ INSTANCIAS ============
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============ VARIABLES GLOBALES ============
unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 30000;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Estaci�n Meteorol�gica IoT ===");

  // Inicializar sensores
  dht.begin();
  lcd.init();
  lcd.backlight();

  if (!bmp.begin()) {
    Serial.println("Error: BMP180 no detectado");
    lcd.setCursor(0, 0);
    lcd.print("BMP180 ERROR");
  } else {
    Serial.println("BMP180 OK");
  }

  // Conectar WiFi
  conectarWiFi();

  // Mostrar mensaje inicial en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Conectando...");
}

void loop() {
  // Verificar conexi�n WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, reconectando...");
    conectarWiFi();
    return;
  }

  // Enviar datos cada 30 segundos
  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();
    enviarLectura();
  }

  delay(1000);
}

void conectarWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(SSID, PASSWORD);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi OK");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  } else {
    Serial.println("\nError: No se pudo conectar WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi ERROR");
  }
}

void enviarLectura() {
  // Leer sensores
  float tempDHT = dht.readTemperature();
  float humedad = dht.readHumidity();
  float tempBMP = bmp.readTemperature();
  float presion = bmp.readPressure();
  int humedadSueloRaw = analogRead(FC28_PIN);
  float humedadSuelo = map(humedadSueloRaw, 0, 4095, 100, 0);

  // Validar NaN
  if (isnan(tempDHT) || isnan(humedad)) {
    Serial.println("Error: Lectura DHT11 fallida");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT ERROR");
    return;
  }

  if (isnan(tempBMP) || isnan(presion)) {
    Serial.println("Error: Lectura BMP180 fallida");
  }

  // Determinar estado del suelo
  String estadoSuelo = "Seco";
  if (humedadSuelo > 70) {
    estadoSuelo = "Mojado";
  } else if (humedadSuelo > 30) {
    estadoSuelo = "Humedo";
  }

  // Mostrar en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempDHT, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hume: ");
  lcd.print(humedad, 1);
  lcd.print("%");

  // Construir JSON
  StaticJsonDocument<256> doc;
  doc["device_id"] = DEVICE_ID;
  doc["temperatura_dht"] = round(tempDHT * 10) / 10.0;
  doc["humedad_ambiente"] = round(humedad * 10) / 10.0;
  doc["temperatura_bmp"] = round(tempBMP * 10) / 10.0;
  doc["presion"] = (int)presion;
  doc["humedad_suelo"] = round(humedadSuelo * 10) / 10.0;
  doc["estado_suelo"] = estadoSuelo;

  // Serializar JSON
  String jsonString;
  serializeJson(doc, jsonString);

  Serial.println("Enviando datos...");
  Serial.println(jsonString);

  // Enviar HTTP POST
  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", API_KEY);

  int httpCode = http.POST(jsonString);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Response [");
    Serial.print(httpCode);
    Serial.print("]: ");
    Serial.println(response);
  } else {
    Serial.print("Error HTTP: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
}

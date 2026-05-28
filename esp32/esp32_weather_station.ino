#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define FC28_ANALOGICO 34

// Configuración WiFi
const char* ssid = "DESKTOP-9577VQQ 9323";
const char* password = "S065=z35";

// Configuración API
const char* serverUrl = "http://192.168.1.100:8000/api/lecturas/";
const char* apiKey = "ESP32_API_KEY_2024";
const char* deviceId = "ESP32_01";

unsigned long lastSend = 0;
const unsigned long sendInterval = 30000;  // 30 segundos

TwoWire I2C_BMP = TwoWire(1);   // BMP180 -> SDA=21, SCL=22

hd44780_I2Cexp lcd(0x27);       // LCD -> SDA=18, SCL=19
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

int valorSeco = 3200;           // Ajusta con tu calibración real
int valorMojado = 1400;         // Ajusta con tu calibración real
int pantalla = 0;

bool bmpOK = false;

void setup() {
  Serial.begin(115200);

  Wire.begin(18, 19);           // LCD
  I2C_BMP.begin(21, 22);        // BMP180

  lcd.begin(16, 2);
  lcd.backlight();

  dht.begin();
  pinMode(FC28_ANALOGICO, INPUT);

  bmpOK = bmp.begin(BMP085_STANDARD, &I2C_BMP);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Estacion Met");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);

  if (!bmpOK) {
    Serial.println("No se encontro el BMP180");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BMP180 fallo");
    lcd.setCursor(0, 1);
    lcd.print("Seguimos...");
    delay(2000);
  }

  // Conectar WiFi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");
  WiFi.mode(WIFI_STA);

  // Escanear redes visibles
  lcd.setCursor(0, 1);
  lcd.print("Escaneando...");
  int redes = WiFi.scanNetworks();
  Serial.print("Redes encontradas: ");
  Serial.println(redes);
  bool encontrada = false;
  for (int i = 0; i < redes; i++) {
    Serial.print("  ");
    Serial.println(WiFi.SSID(i));
    if (WiFi.SSID(i) == ssid) encontrada = true;
  }
  if (!encontrada) {
    Serial.println("ERROR: No se ve la red '" + String(ssid) + "'");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No ve la red:");
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    delay(4000);
  }

  WiFi.begin(ssid, password);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 60) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(".");
    Serial.print(".");
    intentos++;
  }
  Serial.println();
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status());
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi OK");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    Serial.print("Conectado WiFi. IP: ");
    Serial.println(WiFi.localIP());
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi fallo");
    lcd.setCursor(0, 1);
    lcd.print("Modo local");
    delay(2000);
  }
}

void enviarLectura(float t, float h, float tempBmp, int presBmp, int humedadSuelo, String estadoSuelo) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi no conectado, no se envia lectura");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", apiKey);

  StaticJsonDocument<256> doc;
  doc["device_id"] = deviceId;
  doc["temperatura_dht"] = isnan(t) ? 0 : t;
  doc["humedad_ambiente"] = isnan(h) ? 0 : h;
  doc["temperatura_bmp"] = tempBmp;
  doc["presion"] = presBmp;
  doc["humedad_suelo"] = humedadSuelo;
  doc["estado_suelo"] = estadoSuelo;

  String jsonString;
  serializeJson(doc, jsonString);

  int httpCode = http.POST(jsonString);

  Serial.print("Enviando lectura... HTTP ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Respuesta: " + response);
  }

  http.end();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int valorSuelo = analogRead(FC28_ANALOGICO);
  int humedadSuelo = map(valorSuelo, valorSeco, valorMojado, 0, 100);
  humedadSuelo = constrain(humedadSuelo, 0, 100);

  String estadoSuelo = (humedadSuelo < 50) ? "Seco" : "Humedo";

  float tempBmp = 0.0;
  int presBmp = 0;

  if (bmpOK) {
    tempBmp = bmp.readTemperature();
    presBmp = bmp.readPressure();
  }

  // Enviar datos cada sendInterval milisegundos
  if (millis() - lastSend >= sendInterval) {
    enviarLectura(t, h, tempBmp, presBmp, humedadSuelo, estadoSuelo);
    lastSend = millis();
  }

  // Enviar JSON por Serial para el puente USB
  StaticJsonDocument<256> serialDoc;
  serialDoc["device_id"] = deviceId;
  serialDoc["temperatura_dht"] = isnan(t) ? 0 : t;
  serialDoc["humedad_ambiente"] = isnan(h) ? 0 : h;
  serialDoc["temperatura_bmp"] = tempBmp;
  serialDoc["presion"] = presBmp;
  serialDoc["humedad_suelo"] = humedadSuelo;
  serialDoc["estado_suelo"] = estadoSuelo;
  serializeJson(serialDoc, Serial);
  Serial.println();

  lcd.clear();

  if (pantalla == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(isnan(t) ? 0 : t, 1);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(isnan(h) ? 0 : h, 1);
    lcd.print(" %");
  }
  else if (pantalla == 1) {
    lcd.setCursor(0, 0);
    lcd.print("TmpB:");
    lcd.print(bmpOK ? tempBmp : 0, 1);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Pres:");
    lcd.print(bmpOK ? presBmp : 0);
    lcd.print("Pa");
  }
  else if (pantalla == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Suelo: ");
    lcd.print(humedadSuelo);
    lcd.print(" %");

    lcd.setCursor(0, 1);
    lcd.print("Estado: ");
    lcd.print(estadoSuelo);
    if (estadoSuelo == "Seco") {
      lcd.print("   ");
    } else {
      lcd.print(" ");
    }
  }

  Serial.print("Hum ambiente: ");
  Serial.print(h);
  Serial.println(" %");

  Serial.print("Temp DHT11: ");
  Serial.print(t);
  Serial.println(" C");

  Serial.print("Valor analogico suelo: ");
  Serial.println(valorSuelo);

  Serial.print("Hum suelo: ");
  Serial.print(humedadSuelo);
  Serial.println(" %");

  Serial.print("Estado suelo: ");
  Serial.println(estadoSuelo);

  if (bmpOK) {
    Serial.print("Temp BMP180: ");
    Serial.print(tempBmp);
    Serial.println(" C");

    Serial.print("Presion: ");
    Serial.print(presBmp);
    Serial.println(" Pa");
  } else {
    Serial.println("BMP180 no disponible");
  }

  Serial.println("-------------------");

  pantalla++;
  if (pantalla > 2) pantalla = 0;

  delay(3000);
}
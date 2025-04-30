#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>  // Biblioteca para parse de JSON

#define SS_PIN 5
#define RST_PIN 27

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(12, 14, 27, 26, 25, 33);  // ajuste conforme seu LCD

const char* ssid = "João Augusto";
const char* password = "131103r7";
const char* server = "https://verbose-parakeet-6j95ggjrjv93rjrv-3000.app.github.dev/consulta?uid=";

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.begin(16, 2);
  lcd.print("Conectando WiFi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  lcd.print("WiFi Conectado!");
  delay(1000);
  lcd.clear();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.print("UID: ");
  Serial.println(uid);

  lcd.clear();
  lcd.print("UID:");
  lcd.setCursor(0, 1);
  lcd.print(uid);
  delay(1000);

  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(server + uid);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(payload);

      // Parse do JSON para extrair o nome
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
        lcd.clear();
        lcd.print("Erro JSON");
        return;
      }

      const char* nome = doc["nome"];  // Extrai o nome do JSON

      // Exibe a mensagem formatada no LCD
      lcd.clear();
      lcd.print("Ola, ");
      lcd.print(nome);
      lcd.print(" :)");
    } else {
      lcd.clear();
      lcd.print("Erro HTTP!");
    }
    http.end();
  }

  delay(3000);
  lcd.clear();
}
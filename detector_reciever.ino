#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_now.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

typedef struct struct_message {
  float decibels;
} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Decibels: ");
  Serial.println(myData.decibels);
  
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Nivel de Sonido: ");
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.print(myData.decibels, 1);
  display.print(" dB");

  String soundLevel;
  if (myData.decibels < 60) {
    soundLevel = "Normal";
  } else if (myData.decibels < 80) {
    soundLevel = "Medio";
  } else {
    soundLevel = "Alto";
  }

  display.setTextSize(1);
  display.setCursor(0, 48);
  display.print("Nivel: ");
  display.print(soundLevel);

  display.display();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  WiFi.mode(WIFI_STA);
  Serial.println("ESP32 en modo estación");


  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW inicializado correctamente");


  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

}

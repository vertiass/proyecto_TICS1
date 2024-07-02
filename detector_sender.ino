#include <esp_now.h>
#include <WiFi.h>

#define SOUND_SENSOR_PIN 34
const int sampleWindow = 5;

uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDC, 0x14, 0x94};

typedef struct struct_message {
  float decibels;
} struct_message;

struct_message myData;

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.println("ESP32 en modo estación");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW inicializado correctamente");
  
  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer agregado correctamente");

  pinMode(SOUND_SENSOR_PIN, INPUT);
}

void loop() {
  unsigned long startMillis = millis();
  float peakToPeak = 0;                  

  unsigned int signalMax = 0;           
  unsigned int signalMin = 4095;         

  while (millis() - startMillis < sampleWindow) {
    int sample = analogRead(SOUND_SENSOR_PIN);
    if (sample < 4095) {                        
      if (sample > signalMax) {
        signalMax = sample;
      }
      if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = amplitud peak-to-peak
  
  // Convertir peak-to-peak a decibeles usando un mapeo
  float decibels = mapFloat(peakToPeak, 20, 900, 49.5, 90); 

  myData.decibels = decibels;
  
  // Enviar datos al receptor
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  delay(500);
}

#include <esp_now.h>
#include <WiFi.h>

// Define the analog pin where KY-037 AO is connected
#define SOUND_SENSOR_PIN 34

// Define the MAC address of the receiver ESP32
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDC, 0x14, 0x94};

typedef struct struct_message {
  float voltage;
  float dB;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  pinMode(SOUND_SENSOR_PIN, INPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Leer el valor analogo desde el sensor de sonido
  int sensorValue = analogRead(SOUND_SENSOR_PIN);

  // Convertir el valor analogo a voltaje
  float voltage = sensorValue * (3.3 / 4095.0);

  // Convertir el voltaje a decibeles
  float dB = 20 * log10(voltage / 0.00631); // 0.00631 es un valor de voltaje de referencia, necesita calibracion

  Serial.println(dB);

  myData.voltage = voltage;
  myData.dB = dB;

  // Enviar los datos mediante protocolo ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  delay(250);
}

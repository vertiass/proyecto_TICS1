#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  float voltage;
  float dB;
} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Voltage: ");
  Serial.println(myData.voltage);
  Serial.print("dB: ");
  Serial.println(myData.dB);
  Serial.println("<<<<<");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  /*--------Por hacer-----------
  Enviar los datos via HTTP a la pagina web, de manera que se puedan
  visualizar los decibeles.
  */
}

void loop() {

}

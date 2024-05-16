#include <esp_now.h>
#include <WiFi.h>

// Define the analog pin where KY-037 AO is connected
#define SOUND_SENSOR_PIN 34

// Define the MAC address of the receiver ESP32
uint8_t receiverAddress[] = {0xA0, 0xB7, 0x65, 0xDC, 0x14, 0x94};

String success;

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
  if(status == 0) {
    success = "Delivery Success";
  } else {
    success = "Delivery Failed";
  }
}

void setup() {
  Serial.begin(115200);
  // Initialize the pin as input
  pinMode(SOUND_SENSOR_PIN, INPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register the peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Read the analog value from the sound sensor
  int sensorValue = analogRead(SOUND_SENSOR_PIN);

  // Convert the analog value to voltage
  float voltage = sensorValue * (3.3 / 4095.0);

  // Convert the voltage to decibels (dB)
  float dB = 20 * log10(voltage / 0.00631); // 0.00631 is a reference voltage value, needs calibration

  Serial.println(dB);

  // Prepare the data to send
  myData.voltage = voltage;
  myData.dB = dB;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  // Small delay to slow down the reading
  delay(250);
}

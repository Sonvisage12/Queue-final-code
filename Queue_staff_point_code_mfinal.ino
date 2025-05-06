#include <WiFi.h>
#include <esp_now.h>

// ESP-IDF header must be wrapped to avoid C++ conflicts
extern "C" {
  #include "esp_wifi.h"
}

// Replace with the actual MAC address of the Doctor Node (Receiver)
uint8_t doctorMAC[] = {0x68, 0xC6, 0x3A, 0xFC, 0x61, 0x3E};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("📤 Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivered 🟢" : "Failed 🔴");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);  // Must be set before calling esp_wifi functions
  WiFi.disconnect();

  // Set WiFi to promiscuous and force to channel 1
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  Serial.print("📡 Patient MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, doctorMAC, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(doctorMAC)) {
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("❌ Failed to add peer");
      return;
    }
  }

  esp_now_register_send_cb(onDataSent);

  Serial.println("✅ Sending test message in 3 seconds...");
  delay(3000);

  const char *msg = "Hello Doctor!";
  esp_now_send(doctorMAC, (uint8_t *)msg, strlen(msg));
}

void loop() {
  // Nothing
}

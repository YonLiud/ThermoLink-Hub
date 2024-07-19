#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

#define SLAVE_SELECT_PIN 5
#define RESET_PIN 14
#define DIO0_PIN 2


void setup() {
  Serial.begin(115200);

  LoRa.setPins(SLAVE_SELECT_PIN, RESET_PIN, DIO0_PIN); // Initialize LoRa with the pins
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa FAIL!");
    while (1);
  }
  LoRa.setSyncWord(0xA5);
  LoRa.setGain(6);
  Serial.println("LoRa OK!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    // Received a packet
    Serial.print("Received packet '");

    // Read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // Print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

  delay(100);
}
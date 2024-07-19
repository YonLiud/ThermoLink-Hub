#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

#define SLAVE_SELECT_PIN 5
#define RESET_PIN 14
#define DIO0_PIN 2

void parsePacket(byte packet[]);

struct Packet {
    uint8_t SOP;          // Start of Packet (1 byte)
    uint8_t identifier;   // Identifier (1 byte)
    uint16_t randomNumber; // Random Number (2 bytes)
    float temperature;    // Temperature (4 bytes)
};

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
  if (packetSize == sizeof(Packet)) {
    // Received a packet
    Serial.println("");
    Serial.println("Received packet OK!");

    Packet packet;
    LoRa.readBytes((uint8_t*)&packet, sizeof(packet));

    // Verify SOP
    if (packet.SOP != 0x7E) {
      Serial.println("Invalid SOP");
      return;
    }

    packet.temperature = packet.temperature / 100.0;

    Serial.println("");
    Serial.print("ID: ");
    Serial.print(packet.identifier, HEX);
    Serial.print(" - Random Number: ");
    Serial.print(packet.randomNumber, HEX);
    Serial.print(" - Temperature: ");
    Serial.print(packet.temperature);
    // RSSI
    Serial.print(" / RSSI: ");
    Serial.print(LoRa.packetRssi());
  }

  delay(100);
}
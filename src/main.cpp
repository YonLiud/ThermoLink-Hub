#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

// SPI Pin Definition
#define MASTERINSLAVEOUT_PIN 15
#define DIO0_PIN 2
#define SCK_PIN 4
#define MASTEROUTSLAVEIN_PIN 16
#define RESET_PIN 17
#define SLAVESELECT_PIN 5

// I2C Pin Definition
#define SDA_PIN 21
#define SCL_PIN 19

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void parsePacket(byte packet[]);

struct Packet {
    uint8_t SOP;          // Start of Packet (1 byte)
    uint8_t identifier;   // Identifier (1 byte)
    uint16_t randomNumber; // Random Number (2 bytes)
    float temperature;    // Temperature (4 bytes)
};

void setup() {
  Serial.begin(115200);
  
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("Wire OK!");

  SPI.begin(SCK_PIN, MASTERINSLAVEOUT_PIN, MASTEROUTSLAVEIN_PIN, SLAVESELECT_PIN);
  Serial.println("SPI OK!");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation FAIL!"));
    for (;;);
  }
  Serial.println("SSD1306 OK!");

  Serial.println("Displaying splash screen...");
  delay(2000);
  display.display();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SSD1306 OK!");
  display.display();
  

  LoRa.setPins(SLAVESELECT_PIN, RESET_PIN, DIO0_PIN); // Initialize LoRa with the pins
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa FAIL!");
    display.println("LoRa FAIL!");
    display.display();
    while (1);
  }
  LoRa.setSyncWord(0xA5); 
  LoRa.setGain(6);
  Serial.println("LoRa OK!");
  display.println("LoRa OK!");

  display.println("Unconfigured loop method for SSD1306 :)");

  display.display();
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
    Serial.print("ID: ");
    Serial.print(packet.identifier, HEX);
    Serial.print(" - Random Number: ");
    Serial.print(packet.randomNumber, HEX);
    Serial.print(" - Temperature: ");
    Serial.print(packet.temperature);
    // RSSI
    Serial.print(" / RSSI: ");
    Serial.println(LoRa.packetRssi());

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Received packet OK!");
    display.print("ID: ");
    display.println(packet.identifier, HEX);
    display.print("Random Number: ");
    display.println(packet.randomNumber, HEX);
    display.print("Temperature: ");
    display.println(packet.temperature);
    display.print("RSSI: ");
    display.println(LoRa.packetRssi());
    display.display();

  }

  delay(100);
}
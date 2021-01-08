#include <ESP8266WiFi.h>
#include <NeoPixelBus.h>
#include <SPI.h>
#include <WiFiUdp.h>

#include "tpm2net.h"

// == WIFI CONFIG =========================
#define WIFI_SSID "ATTWFaYg8s"
#define WIFI_PASS "q#2tgx#j+2d6"
IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

#define UDP_PORT 6999
WiFiUDP udp;
// ========================================

// == TPM2 Protocol =======================
// https://gist.github.com/jblang/89e24e2655be6c463c56

#define TPM2NET_HEADER_SIZE 5
// #define TPM2NET_HEADER_IDENT 0x9c
// #define TPM2NET_CMD_DATAFRAME 0xda
// #define TPM2NET_CMD_COMMAND 0xc0
// #define TPM2NET_CMD_ANSWER 0xaa
// #define TPM2NET_FOOTER_IDENT 0x36

// as the arduino ethernet has only 2kb ram
// we must limit the maximal udp packet size
// a 64 pixel matrix needs 192 bytes data
#define UDP_PACKET_SIZE 500

// ========================================

// == Neopixel CONFIG =====================
//...
// ========================================

void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.println();

    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    Serial.println("Starting UDP");
    udp.begin(UDP_PORT);
    Serial.printf("Listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), UDP_PORT);
}

void loop() {
    // if there's data available, read a packet
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
    }
    // //tpm2 header size is 5 bytes
    //    if (packetSize > EXPECTED_PACKED_SIZE) {

    // // read the packet into packetBufffer
    // udp.read(packetBuffer, UDP_PACKET_SIZE);

    // // -- Header check

    // //check header byte
    // if (packetBuffer[0] != TPM2NET_HEADER_IDENT) {
    //   Serial.print("Invalid header ident ");
    //   Serial.println(packetBuffer[0], HEX);
    //   return;
    // }

    // //check command
    // if (packetBuffer[1] != TPM2NET_CMD_DATAFRAME) {
    //   Serial.print("Invalid block type ");
    //   Serial.println(packetBuffer[1], HEX);
    //   return;
    // }

    // uint16_t frameSize = packetBuffer[2];
    // frameSize = (frameSize << 8) + packetBuffer[3];
    // Serial.print("Framesize ");
    // Serial.println(frameSize, HEX);

    // //use packetNumber to calculate offset
    // uint8_t packetNumber = packetBuffer[4];
    // Serial.print("packetNumber ");
    // Serial.println(packetNumber, HEX);

    // //check footer
    // if (packetBuffer[frameSize + TPM2NET_HEADER_SIZE] != TPM2NET_FOOTER_IDENT) {
    //   Serial.print("Invalid footer ident ");
    //   Serial.println(packetBuffer[frameSize + TPM2NET_HEADER_SIZE], HEX);
    //  // return;
    // }

    // //calculate offset
    // uint16_t currentLed = packetNumber * PIXELS_PER_PANEL;
    // int x = TPM2NET_HEADER_SIZE;
    // for (byte i = 0; i < frameSize; i++) {
    //    strip.SetPixelColor(currentLed++, packetBuffer[x], packetBuffer[x + 1], packetBuffer[x
    //    + 2]);
    //   x += 3;
    // }

    // //TODO maybe update leds only if we got all pixeldata?
    // strip.Show();   // write all the pixels out
    //    }
    // }
}

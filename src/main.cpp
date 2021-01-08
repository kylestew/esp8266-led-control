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

WiFiUDP udp;
// ========================================

// == TPM2NET PROTOCOL==========================
// https://gist.github.com/jblang/89e24e2655be6c463c56
struct TPM2NET_HEADER {
    byte blockType;
    byte length[2];
    byte packetNumber;
    byte packetCount;
};
// ========================================

// == Neopixel CONFIG =====================
#define PIXEL_PIN 2
#define PIXEL_COUNT 6
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
    udp.begin(TPM2_NET_PORT);
    Serial.printf("Listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(),
                  TPM2_NET_PORT);
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize > 7 && udp.read() == TPM2_NET_BLOCK_START_BYTE) {
        // read header data
        TPM2NET_HEADER header;
        byte tmp[sizeof(header)];
        udp.read(tmp, sizeof(header));
        memcpy(&header, tmp, sizeof(header));

        if (header.blockType != TPM2_BLOCK_TYPE_DATA) {
            Serial.println("Unsupported block type received.");
            return;
        }

        uint16_t payloadLength = word(header.length[0], header.length[1]);
        byte data[payloadLength];
        udp.read(data, payloadLength);

        if (udp.read() == TPM2_BLOCK_END_BYTE) {
            // fin!
            Serial.println("");
            Serial.print(data[0]);
            Serial.print(data[1]);
            Serial.print(data[2]);
        } else {
            Serial.println("Malformed packed received.");
        }
    }
}

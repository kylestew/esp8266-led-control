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
#define PIXEL_COUNT 8
#define PIXEL_PIN 14

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(PIXEL_COUNT, PIXEL_PIN);
// ========================================

void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.println();

    // reset pixels
    pixels.Begin();
    pixels.Show();

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
            int count = payloadLength / 3;
            count = min(count, PIXEL_COUNT);

            pixels.Begin();
            for (int i = 0; i < count; i++) {
                pixels.SetPixelColor(i, RgbColor(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
            }
            pixels.Show();
        } else {
            Serial.println("Malformed packed received.");
        }
    }
}

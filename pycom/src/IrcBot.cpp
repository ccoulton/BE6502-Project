#include <Arduino.h>
#include <WiFi.h>
#include "..\inc\IrcBot.h"
#include "..\inc\auth.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#undef FASTLED_INTERNAL
CRGB led;

inline void scanNetworks() {
    Serial.println("Scan Start");
    int n = WiFi.scanNetworks();
    if (n == 0) {
      Serial.println("No Networks found.");
    } else {
      Serial.printf("%d Networks Found!\n", n);
      for (int index = 0; index < n; index++) {
        Serial.printf("%d: %s (%d)\n", 
          index+1, WiFi.SSID(index).c_str(), 
          WiFi.RSSI(index));
        delay(10);
      }
    }
    led = CRGB::OrangeRed;
    FastLED.show();
    delay(1000);
    led = CRGB::Blue;
    FastLED.show();
    delay(1000);
    led = CRGB::Aqua;
    FastLED.show();
    delay(1000);
    led = CRGB::Green;
    FastLED.show();
    delay(1000);
    led = CRGB::Black;
    FastLED.show();
    delay(1000);
}

void connectToKnownNetworks() {
    int loops;
    //scanNetworks();
    for (int index = 0; index < 4; index++) {
        loops = 10;
        WiFi.begin(KNOWN_NETWORKS[index].ssid, KNOWN_NETWORKS[index].pwd);
        while ((WiFi.status() != WL_CONNECTED) &&  (--loops > 0)) {
            if (WiFi.status() != WL_CONNECT_FAILED) {
                break;
            } else {
                delay(500);
                Serial.print(".");
            }
        }
        if (loops > 0){
            Serial.printf("\nWifi Connected:\n");
            Serial.print("Ip Address: ");
            Serial.println(WiFi.localIP());
            break;
        } else if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Could not connect to network in time.");
            Serial.println("Trying next Network in Known Networks.");
            continue;
        } else { /* Do Nothing */}
    }   
}

/*
#https://dev.twitch.tv/docs/client/guide
#ws://irc-ws.chat.twitch.tv:80
*/
void IrcBotTask(void* parameters) {
    //setup for the wifi task
    Serial1.begin(19200); //6551 ACIA
    (void)TOKEN;
    //Led setup
    FastLED.addLeds<WS2812, BUILTIN_LED, GRB>(&led, 1);
    pinMode(BUILTIN_LED, OUTPUT);
    WiFiClient client;
    if (!client.connect(SERVER, PORT)) {
        //connection failed;
        return;
    }
    
    while(true) {
    //scanNetworks();
    }
}
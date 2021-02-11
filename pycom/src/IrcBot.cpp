#include <Arduino.h>
#include <Regexp.h>
#include <WiFi.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#undef FASTLED_INTERNAL
#include "..\inc\IrcBot.h"
#include "..\inc\auth.h"
CRGB led;
static WiFiClient client;

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
    for (int index = 0; index < NETWORK_NUM; index++) {
        loops = 20;
        Serial.print("Trying to connect to ");
        Serial.println(KNOWN_NETWORKS[index].ssid);
        WiFi.begin(KNOWN_NETWORKS[index].ssid, KNOWN_NETWORKS[index].pwd);
        while ((WiFi.status() != WL_CONNECTED) && (loops-- > 0)){
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        if (WiFi.status() == WL_CONNECTED){
            Serial.println("Wifi Connected!");
            Serial.print("Ip Address: ");
            Serial.println(WiFi.localIP());
            index = 4;
        } else if (loops == 0) {
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
void send_message(const char *msg) {
    char msg_pkt[512];
    sprintf(msg_pkt, "PRIVMSG %s :%s\r\n", CHANNEL, msg);
    client.write(msg_pkt, strlen(msg_pkt));
}

void send_Data(const char *cmd, const char *msg){
    char msg_pkt[512];
    sprintf(msg_pkt, "%s%s\r\n", cmd, msg);
    client.write(msg_pkt);
}

void connectTwitch(void) {
    if (!client.connect(SERVER, PORT)) {
        Serial.println("Connection failed.");
        return;
    }
    Serial.println("Connected to twitch?");
    send_Data("PASS ", TOKEN);
    send_Data("NICK ", NICK);
    char *userString = new char[27];
    sprintf(userString, "User %s 0 * :", BOTOWNER);
    send_Data(userString, BOTOWNER);
    free(userString);
    send_Data("JOIN ", CHANNEL);
}

void IrcBotTask(void* parameters) {
    MatchState ms;
    char matchBuffer[50];
    sprintf(matchBuffer, "%s %s %s", 
        ":(%w*)!.*%.tv (%u*)", CHANNEL, ":!(%a*)");
    while(1) {
        char *line = new char[512];
        if (client.available()) {
            size_t size;
            int result;
            size = client.readBytes(line, 512);
            Serial.println(line);
            ms.Target(line, size);
            result = ms.Match(matchBuffer, 0);
            if (strcmp(line, "PING") == 0) {
                send_Data("PONG :", "tmi.twitch.tv"); //argument.
                send_message("Bot: Hey Thanks for hanging out.");
            } else if(result) {
                char messageString[60];
                char username[26];
                char irccmd[10];
                char command[10];
                ms.GetCapture(username, 0);
                ms.GetCapture(irccmd, 1); //join//etc
                ms.GetCapture(command, 2);
                if (strcmp(irccmd, "PRIVMSG") == 0) {
                    if (strcmp(command, "seen") == 0) {
                        send_message("Bot: I see and Obey!");
                    } else if (strcmp(command, "lurk") == 0) {
                        sprintf(messageString, 
                            "Bot: Ok %s Enjoy your lurk.", username);
                        send_message(messageString);
                    } else if (strcmp(command, "rgbled")) {
                        if (ms.Match(":!rgbled 0x(%x*)", 0)) {
                            ms.GetCapture(command, 0);
                            led = strtoul(command, NULL, 16);
                        } else if(ms.Match(":!rgbled help", 0)) {
                            send_message("Bot: For defined RGB values check 
                                https://github.com/FastLED/FastLED/wiki/Pixel-reference#colors.");
                        } else {
                            sprintf(messageString, 
                                "Bot: Sorry %s RGB was not correct try again.", username);
                            send_message(messageString);
                        }
                    }
                } else if (strcmp(irccmd, "JOIN") == 0) {
                    sprintf(messageString, "Bot: Hello %s thanks for joining!", username);
                    send_message(messageString);//*/
                }
            }
        }
        free(line);
    }
}
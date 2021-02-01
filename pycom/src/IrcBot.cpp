#include <Arduino.h>
#include <WiFi.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#undef FASTLED_INTERNAL
#include "..\inc\IrcBot.h"
#include "..\inc\auth.h"
CRGB led;
static WiFiClient client;

static char *get_command(char line[]) {
    char *command = (char*) malloc(512);
    char clone[512];
    strncpy(clone, line, strlen(line)+1);
    char *splitted = strtok(clone, " ");
    if (splitted != NULL){
        if (splitted[0] == ':'){
            splitted = strtok(NULL, " ");
        }
        if (splitted != NULL){
            strncpy(command, splitted, strlen(splitted)+1);
        }else{
            command[0] = '\0';
        }
    }else{
        command[0] = '\0';
    }
    return command;
}

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
    for (int index = 0; index < 4; index++) {
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
            //Serial.println(WiFi.)
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

void IrcBotTask(void* parameters) {
    if (!client.connect(SERVER, PORT)) {
        Serial.println("Connection failed.");
        return;
    }
    send_Data("PASS ", TOKEN);
    send_Data("NICK ", NICK);
    char *userString = new char[27];
    sprintf(userString, "User %s 0 * :", BOTOWNER);
    send_Data(userString, BOTOWNER);
    free(userString);
    send_Data("JOIN ", CHANNEL);
    while(1) {
        char line[512];
        if (client.available()){
            client.readBytes(line, 512);
            //char *prefix = get_prefix(line);
            //char *userName = get_userName(line);
            char *command = get_command(line);
            //char *argument = get_Last_arg(line);
            if (strcmp(command, "PING") == 0) {
                send_Data("PONG :", "tmi.twitch.tv"); //argument.
                send_message("Bot: Hey Thanks for hanging out.");
            } else if(strcmp(command, "PRIVMSG") == 0) { // 

            } else if(strcmp(command, "JOIN") == 0) { //user name has joined!

            } else { /* Reserved */ }
        }
    }
}
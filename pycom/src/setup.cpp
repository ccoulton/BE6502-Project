#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_MCP23008.h>
#include "..\inc\6502-monitor.h"

#define FASTLED_INTERNAL
#include <FastLED.h>
#undef FASTLED_INTERNAL
#include "WiFi.h"
#include "..\inc\IrcBot.h"


/*#define JTAG_TDO 15 //p4
#define JTAG_TDI 12 //p9
#define JTAG_TCK 13 //p10
#define JTAG_TMS 14 //p23
rx/tx0 = 3/1, rx/tx1 = 15/4, rx/tx2 = 16/17
sda/scl0 = 12/13, sda/scl1 = 21,22?
ws2812/bootloader pin 0, safeboot, 21
sd dat0 2/p8, sclk 14/p23, cmd 15/p4
*/

#undef ANT_SELECT
#define ANT_SELECT 16

void setup() {
    Serial.begin(115200); //USB moniter
    Serial1.begin(19200, SERIAL_8N1, 15, 4, false, 20000UL); //6551 ACIA
    //Setup for the wifi Task.
    connectToKnownNetworks();
    //Led setup
    FastLED.addLeds<WS2812, BUILTIN_LED, GRB>(&led, 1);
    pinMode(BUILTIN_LED, OUTPUT);
    //init the i2c debugger
    Wire.begin(12, 13, 100000);
    ADDR.begin((uint8_t)0); //0x20
    DATA.begin((uint8_t)1); //0x21
    CNTR.begin((uint8_t)2); //0x22

    for (int n = 0; n < 16; n++) {
        ADDR.pinMode(n, INPUT);
    }
    for (int n = 0; n < 8; n++) {
        DATA.pinMode(n, INPUT);
        CNTR.pinMode(n, INPUT);
    }
    //Wire.beginTransmission(0x22);
    //byte buff = 0x01;
    //Wire.writeTransmission(MCP23008_GPINTEN, &buff, 1); //set irq from mcp23008 on pin 0 or clock
    //Wire.endTransmission();
    pinMode(CLOCK, INPUT);
    //arduino code always runs on core 1 so this is pinned there. perfect.
    attachInterrupt(CLOCK, onClock, RISING);//*/
    //arduino defaults to core 1 
    //xTaskCreatePinnedToCore(DebuggerTask, "6502 Debugger", 1000, NULL, 4, NULL, 1);
    //put wifi on core 0 
    connectTwitch();
    xTaskCreatePinnedToCore(IrcBotTask,   "IrcBot", 4000, NULL, 5, NULL, 0);
    disableCore0WDT(); //deals with wdt fault on idle0?
}

void loop(){
    DebuggerTask(NULL);
}
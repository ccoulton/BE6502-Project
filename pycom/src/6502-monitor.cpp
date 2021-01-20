#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <pins_arduino.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#undef FASTLED_INTERNAL
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;
Adafruit_MCP23008 CNTR;
CRGB led;
bool clockFlag = false;

const char opcodeMatrix[256][5] = {\
        "BRK", "ORA", "COP", "ORA", "TSB", "ORA", "ASL", "RMB0", "PHP", "ORA", "ASL", "PHD", "TSB", "ORA", "ASL", "BBR0",\
        "BPL", "ORA", "ORA", "ORA", "TRB", "ORA", "ASL", "RMB1", "CLC", "ORA", "INC", "TCS", "TRB", "ORA", "ASL", "BBR1",\
        "JSR", "AND", "JSL", "AND", "BIT", "AND", "ROL", "RMB2", "PLP", "AND", "ROL", "PLD", "BIT", "AND", "ROL", "BBR2",\
        "BMI", "AND", "AND", "AND", "BIT", "AND", "ROL", "RMB3", "SEC", "AND", "DEC", "TSC", "BIT", "AND", "ROL", "BBR3",\
        "RTI", "EOR", "WDM", "EOR", "MVP", "EOR", "LSR", "RMB4", "PHA", "EOR", "LSR", "PHK", "JMP", "EOR", "LSR", "BBR4",\
        "BVC", "EOR", "EOR", "EOR", "MVN", "EOR", "LSR", "RMB5", "CLI", "EOR", "PHY", "TCD", "JMP", "EOR", "LSR", "BBR5",\
        "RTS", "ADC", "PER", "ADC", "STZ", "ADC", "ROR", "RMB6", "PLA", "ADC", "ROR", "RTL", "JMP", "ADC", "ROR", "BBR6",\
        "BVS", "ADC", "ADC", "ADC", "STZ", "ADC", "ROR", "RMB7", "SEI", "ADC", "PLY", "TDC", "JMP", "ADC", "ROR", "BBR7",\
        "BRA", "STA", "BRL", "STA", "STY", "STA", "STX", "SMB0", "DEY", "BIT", "TXA", "PHB", "STY", "STA", "STX", "BBS0",\
        "BCC", "STA", "STA", "STA", "STY", "STA", "STX", "SMB1", "TYA", "STA", "TXS", "TXY", "STZ", "STA", "STZ", "BBS1",\
        "LDY", "LDA", "LDX", "LDA", "LDY", "LDA", "LDX", "SMB2", "TAY", "LDA", "TAX", "PLB", "LDY", "LDA", "LDX", "BBS2",\
        "BCS", "LDA", "LDA", "LDA", "LDY", "LDA", "LDX", "SMB3", "CLV", "LDA", "TSX", "TYX", "LDY", "LDA", "LDX", "BBS3",\
        "CPY", "CMP", "REP", "CMP", "CPY", "CMP", "DEC", "SMB4", "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "BBS4",\
        "BNE", "CMP", "CMP", "CMP", "PEI", "CMP", "DEC", "SMB5", "CLD", "CMP", "PHX", "STP", "JML", "CMP", "DEC", "BBS5",\
        "CPX", "SBC", "SEP", "SBC", "CPX", "SBC", "INC", "SMB6", "INX", "SBC", "NOP", "XBA", "CPX", "SBC", "INC", "BBS6",\
        "BEQ", "SBC", "SBC", "SBC", "PEA", "SBC", "INC", "SMB7", "SED", "SBC", "PLX", "XCE", "JSR", "SBC", "INC", "BBS7"};
void onClock() {
  clockFlag = true;
  CNTR.readGPIO();
};
void IrcBotTask(void* parameters);
void DebuggerTask(void* parameters);

#define CLOCK 25 //interupt pin though optocouple
void setup() {
  Serial.begin(115200); //USB moniter
  //arduino defaults to core 1 
  xTaskCreatePinnedToCore(DebuggerTask, "6502 Debugger", 1000, NULL, 1, NULL, 1);
  //put wifi on core 0 
  xTaskCreatePinnedToCore(IrcBotTask,   "IrcBot", 1000, NULL, 1, NULL, 0);
}

void IrcBotTask(void* parameters) {
  //setup for the wifi task
  Serial1.begin(19200); //6551 ACIA
  //Led setup
  FastLED.addLeds<WS2812, BUILTIN_LED, GRB>(&led, 1);
  pinMode(BUILTIN_LED, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Wifi Setup Done.");
  while(true) {
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
}

void DebuggerTask(void* parameters) {
  //init the i2c debugger
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
  Wire.beginTransmission(0x22);
  byte buff = 0x01;
  Wire.writeTransmission(MCP23008_GPINTEN, &buff, 1); //set irq from mcp23008 on pin 0 or clock
  Wire.endTransmission();
  pinMode(CLOCK, INPUT);
  //arduino code always runs on core 1 so this is pinned there. perfect.
  attachInterrupt(CLOCK, onClock, RISING);
  for (;;){ //replacement for arduino loop
    if (clockFlag) {
      detachInterrupt(CLOCK);
      clockFlag = false;
      char output[25];

      uint16_t hexaddress = ADDR.readGPIOAB();

      uint8_t hexdata = DATA.readGPIO();
      bool rwb = CNTR.digitalRead(1);
      bool vda = CNTR.digitalRead(2);
      bool vpa = CNTR.digitalRead(3);

      sprintf(output, "%04x %c %02x instr:%4s", hexaddress, 
        rwb ? 'r' : 'W', hexdata,
        (vda&&vpa) ? opcodeMatrix[hexdata]:"EXE");
      Serial.println(output);
      attachInterrupt(CLOCK, onClock, RISING);
    }
  }
}

void loop(){}

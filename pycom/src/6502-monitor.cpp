#include <Arduino.h>
#define SPI_DATA 22
#define SPI_CLOCK 13
#include <FastLED.h>
#include <Wire.h>
#include <pins_arduino.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"
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
void onClock();

#define CLOCK 25 //interupt pin though optocouple
#define SPI
//char aciabuffer[127];
//int aciabufidx = 0;
void setup() {
  ADDR.begin((uint8_t)0);
  DATA.begin((uint8_t)1);
  CNTR.begin((uint8_t)2);
  Serial.begin(115200);
  Serial1.begin(19200);
  FastLED.addLeds<WS2812, BUILTIN_LED, GRB>(&led, 1);

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
  //Wire.writeTransmission(mc);
  pinMode(CLOCK, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  attachInterrupt(CLOCK, onClock, RISING);
}

void onClock() {
  clockFlag = true;
  CNTR.readGPIO();
}

void loop(){
#if 0
  if (Serial1.available()){
    uint8_t incoming = Serial1.read();
    if ((char)incoming == '\r') {
      Serial.println("");
    }
    Serial.print((char)incoming);
  }
  if (Serial.available()){
    uint8_t incoming = Serial.read();
    if ((char)incoming != '\n'){
      Serial1.write((char)incoming);
    }
  }
#endif
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

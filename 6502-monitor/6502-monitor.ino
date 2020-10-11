#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;
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

#define CLOCK 7
#define READ_WRITE 5
#define LED 13
#define VDA 9
#define RDY 10
#define IRQ 11
#define VPA 12
SoftwareSerial aciaSerial(MOSI, MISO);
char buffer[20];
int bufidx = 0;
void setup() {
  ADDR.begin((uint8_t)0);
  DATA.begin((uint8_t)1);
  Serial.begin(9600);
  aciaSerial.begin(19200);
  for (int n = 0; n < 16; n++) {
    ADDR.pinMode(n, INPUT);
  }
  for (int n = 0; n < 8; n++) {
    DATA.pinMode(n, INPUT);
  }
  pinMode(CLOCK, INPUT);
  pinMode(READ_WRITE, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(VPA, INPUT);
  pinMode(VDA, INPUT);
  pinMode(RDY, OUTPUT);
  //digitalWrite(RDY, HIGH);
  pinMode(IRQ, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
}

void onClock() {
  clockFlag = true;
}

void loop(){
  if (aciaSerial.available()){
    char incoming = aciaSerial.read();
    buffer[bufidx++] = incoming;
    if (incoming == 0x0a) {
      //digitalWrite(RDY, LOW);
      Serial.println(buffer);
      while(!Serial.available()){
        delay(1000);
      }
      aciaSerial.write(Serial.read());
      //digitalWrite(RDY, HIGH);
    }
  }
  if (clockFlag) {
    detachInterrupt(digitalPinToInterrupt(CLOCK));
    clockFlag = false;
    char output[25];

    uint16_t hexaddress = ADDR.readGPIOAB();

    uint8_t hexdata = DATA.readGPIO();

    sprintf(output, "%04x %c %02x instr:%4s", hexaddress, 
      digitalRead(READ_WRITE) ? 'r' : 'W', hexdata,
      digitalRead(VDA&&VPA) ? opcodeMatrix[hexdata]:"EXE");
    Serial.println(output);
    attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
  }
}

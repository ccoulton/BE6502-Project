#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;
bool clockFlag = false;

const char opcodeMatrix[256][5] = {\
             "BRK", "ORA", ""   , "", "TSB", "ORA", "ASL", "RMB0", "PHP", "ORA", "ASL", ""   , "TSB", "ORA", "ASL", "BBR0",\
             "BPL", "ORA", "ORA", "", "TRB", "ORA", "ASL", "RMB1", "CLC", "ORA", "INC", ""   , "TRB", "ORA", "ASL", "BBR1",\
             "JSR", "AND", ""   , "", "BIT", "AND", "ROL", "RMB2", "PLP", "AND", "ROL", ""   , "BIT", "AND", "ROL", "BBR2",\
             "BMI", "AND", "AND", "", "BIT", "AND", "ROL", "RMB3", "SEC", "AND", "DEC", ""   , "BIT", "AND", "ROL", "BBR3",\
             "RTI", "EOR", ""   , "", ""   , "EOR", "LSR", "RMB4", "PHA", "EOR", "LSR", ""   , "JMP", "EOR", "LSR", "BBR4",\
             "BVC", "EOR", "EOR", "", ""   , "EOR", "LSR", "RMB5", "CLI", "EOR", "PHY", ""   , ""   , "EOR", "LSR", "BBR5",\
             "RTS", "ADC", ""   , "", "STZ", "ADC", "ROR", "RMB6", "PLA", "ADC", "ROR", ""   , "JMP", "ADC", "ROR", "BBR6",\
             "BVS", "ADC", "ADC", "", "STZ", "ADC", "ROR", "RMB7", "SEI", "ADC", "PLY", ""   , "JMP", "ADC", "ROR", "BBR7",\
             "BRA", "STA", ""   , "", "STY", "STA", "STX", "SMB0", "DEY", "BIT", "TXA", ""   , "STY", "STA", "STX", "BBS0",\
             "BCC", "STA", "STA", "", "STY", "STA", "STX", "SMB1", "TYA", "STA", "TXS", ""   , "STZ", "STA", "STZ", "BBS1",\
             "LDY", "LDA", "LDX", "", "LDY", "LDA", "LDX", "SMB2", "TAY", "LDA", "TAX", ""   , "LDY", "LDA", "LDX", "BBS2",\
             "BCS", "LDA", "LDA", "", "LDY", "LDA", "LDX", "SMB3", "CLV", "LDA", "TSX", ""   , "LDY", "LDA", "LDX", "BBS3",\
             "CPY", "CMP", ""   , "", "CPY", "CMP", "DEC", "SMB4", "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "BBS4",\
             "BNE", "CMP", "CMP", "", ""   , "CMP", "DEC", "SMB5", "CLD", "CMP", "PHX", "STP", ""   , "CMP", "DEC", "BBS5",\
             "CPX", "SBC", ""   , "", "CPX", "SBC", "INC", "SMB6", "INX", "SBC", "NOP", ""   , "CPX", "SBC", "INC", "BBS6",\
             "BEQ", "SBC", "SBC", "", ""   , "SBC", "INC", "SMB7", "SED", "SBC", "PLX", ""   , ""   , "SBC", "INC", "BBS7"};

#define CLOCK 7
#define READ_WRITE 5
#define LED 13
#define SYNC 9
SoftwareSerial aciaSerial(MISO, MOSI);
void setup() {
  ADDR.begin(0);
  DATA.begin(1);
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
  pinMode(SYNC, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
}

void onClock() {
  clockFlag = true;
}

void loop(){
  if (aciaSerial.available()){
    char incoming = aciaSerial.read();
    Serial.print(incoming);
    aciaSerial.write("\n");
  }
  if (clockFlag) {
    detachInterrupt(digitalPinToInterrupt(CLOCK));
    clockFlag = false;
    char output[25];

    uint16_t hexaddress = ADDR.readGPIOAB();

    uint8_t hexdata = DATA.readGPIO();

    sprintf(output, "%04x %c %02x instr:%4s", hexaddress, digitalRead(READ_WRITE) ? 'r' : 'W', hexdata,
      digitalRead(SYNC)?opcodeMatrix[hexdata]:"EXE");
    Serial.println(output);
    attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
  }
}

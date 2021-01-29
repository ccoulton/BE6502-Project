#include <Wire.h>
#include "..\inc\6502-monitor.h"
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;
Adafruit_MCP23008 CNTR;
static bool clockFlag = false;

void onClock() {
  clockFlag = true;
};

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
  //Wire.beginTransmission(0x22);
  //byte buff = 0x01;
  //Wire.writeTransmission(MCP23008_GPINTEN, &buff, 1); //set irq from mcp23008 on pin 0 or clock
  //Wire.endTransmission();
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

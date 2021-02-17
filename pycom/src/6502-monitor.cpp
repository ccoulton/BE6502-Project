#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_MCP23008.h>
#include "..\inc\6502-monitor.h"
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;
Adafruit_MCP23008 CNTR;
static bool clockFlag = false;

void onClock() {
  clockFlag = true;
  //xTaskCreatePinnedToCore(DebuggerTask, "6502 Debugger", 2000, NULL, 4, NULL, 1);
};

void DebuggerTask(void* parameters) {
  //for (;;){ //replacement for arduino loop
  if (digitalRead(CLOCK)) {
    //detachInterrupt(CLOCK);
    //clockFlag = false;
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
    //vTaskDelete(NULL);
    //attachInterrupt(CLOCK, onClock, RISING);
  }
  //}
}

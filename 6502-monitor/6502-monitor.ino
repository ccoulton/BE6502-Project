#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"
/*const char ADDR[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
const char DATA[] = {39, 41, 43, 45, 47, 49, 51, 53};*/
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;
bool clockFlag = false;

#define CLOCK 7
#define READ_WRITE 5
#define LED 13

void setup() {
  ADDR.begin(0);
  DATA.begin(1);
  Serial.begin(57600);
  for (int n = 0; n < 16; n++) {
    ADDR.pinMode(n, INPUT);
  }
  for (int n = 0; n < 8; n++) {
    DATA.pinMode(n, INPUT);
  }
  pinMode(CLOCK, INPUT);
  pinMode(READ_WRITE, INPUT);
  pinMode(LED, OUTPUT);
}

void onClock() {
  clockFlag = true;
}

void loop(){
  if (clockFlag) {
    detachInterrupt(digitalPinToInterrupt(CLOCK));
    clockFlag = false;
    char output[15];

    /*unsigned int address = 0;
    for (int n = 15; n >= 0; n--) {
      int bit = ADDR.digitalRead(n) ? 1 : 0;
      Serial.print(bit);
      address = (address << 1) + bit;
    }//*/
    uint16_t hexaddress = ADDR.readGPIOAB();
    //Serial.print("  ");

    /*unsigned int data = 0;
    for (int n = 7; n >= 0; n--) {
      int bit = DATA.digitalRead(n) ? 1 : 0;
      Serial.print(bit);
      data = (data << 1) + bit;
    }//*/

    uint8_t hexdata = DATA.readGPIO();

    //sprintf(output, "   %04x  %c %02x", address, digitalRead(READ_WRITE) ? 'r' : 'W', data);
    //Serial.println(output);
    sprintf(output, "%04x %c %02x", hexaddress, digitalRead(READ_WRITE) ? 'r' : 'W', hexdata);
    Serial.println(output);
    attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
  }
}

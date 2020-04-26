#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_MCP23008.h"

#define WRITE_EN A2
#define OUTPUT_EN A1
#define CHIP_EN A0
#define EEPROM_D0 0
#define EEPROM_D7 7
Adafruit_MCP23017 ADDR;
Adafruit_MCP23008 DATA;

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(uint16_t address) {
  ADDR.writeGPIOAB(address);
  /*shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);*/
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(uint16_t address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++) {
    DATA.pinMode(pin, INPUT);
  }
  setAddress(address);

  digitalWrite(WRITE_EN, HIGH);
  digitalWrite(OUTPUT_EN, LOW);
  digitalWrite(CHIP_EN, LOW);
  byte data = DATA.readGPIO();
  digitalWrite(OUTPUT_EN, HIGH);
  digitalWrite(CHIP_EN, HIGH);
  delay(1);

  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(uint16_t address, byte data) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++) {
    DATA.pinMode(pin, OUTPUT);
  }
  setAddress(address);
  
  digitalWrite(OUTPUT_EN, HIGH);
  digitalWrite(CHIP_EN, LOW);
  digitalWrite(WRITE_EN, LOW);
  DATA.writeGPIO(data);
  delay(1);

  digitalWrite(CHIP_EN, HIGH);
  digitalWrite(WRITE_EN, HIGH);
  delay(1);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset++) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


// 4-bit hex decoder for common anode 7-segment display
byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };

// 4-bit hex decoder for common cathode 7-segment display
// byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };


void setup() {
  ADDR.begin(0);
  DATA.begin(2);
  for (int n = 0; n < 16; n++) {
    ADDR.pinMode(n, OUTPUT);
  }
  pinMode(OUTPUT_EN, OUTPUT);
  pinMode(CHIP_EN, OUTPUT);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);
  while(!Serial){;} //wait for serial
  // Erase entire EEPROM
  digitalWrite(OUTPUT_EN, HIGH);
  digitalWrite(CHIP_EN, HIGH);
  digitalWrite(WRITE_EN, HIGH);
  delay(1);

  Serial.print("Erasing EEPROM");
  for (uint16_t address = 0; address < 0x7fff; address++) {
    writeEEPROM(address, 0xff);

    if (address % 0x3f == 0) {
      Serial.print(".");
    }
    if (address % 0x03ff == 0){
      Serial.print("  ");
    }
    if (address % 0x07ff == 0){
      Serial.println("");
    }
  }
  Serial.println(" done!");


  // Program data bytes
  Serial.print("Programming EEPROM");
  for (uint16_t address = 0; address < sizeof(data); address++) {
    writeEEPROM(address, data[address]);
    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // put your main code here, to run repeatedly:
}

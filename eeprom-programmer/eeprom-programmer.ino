#include <Arduino.h>
//0->7
//portb ss/nc, sck, mosi, miso, 8, 9, 10, 11
//portc nc, nc, nc, nc, nc, 5, 13
//portd 3, 2, 0, 1, 4, nc, 12, 6
//porte nc, nc, nc, nc, nc, nc, 7,  
//portf a5, a4, nc, nc, a3, a2, a1, a0
#define SHIFT_DATA 5
#define SHIFT_CLK 2
#define SHIFT_LATCH 3
#define WRITE_EN 7
#define EEPROM_D0 0
#define EEPROM_D7 7

int DATA_PIN[] = {A0, A1, A2, A3, A4, A5, 9, 10};
/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);
  //trigger the shift register to trigger all outputs at once rather than as they are clocked in.
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
  /*PORTD &= 0b00000000; //latch low
  //delayMicroseconds(1);
  PORTD &= 0b00000001; //latch high
  //delayMicroseconds(1);
  PORTD &= 0b00000000; //latch low
  //*/
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(DATA_PIN[pin], INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(DATA_PIN[pin]);
  }
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(DATA_PIN[pin], data & 1);
    data = data >> 1;
  }
  //digitalWrite(WRITE_EN, LOW);
  //delayMicroseconds(1);
  //digitalWrite(WRITE_EN, HIGH);
  //delayMicroseconds(1);  
  PORTE &= ~(1<<6);
  delayMicroseconds(1);
  PORTE |= (1<<6);
  //delayMicroseconds(1);
  //*/
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
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
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(DATA_PIN[pin], OUTPUT);
  }
  Serial.begin(57600);
  while(!Serial){;}
  // Erase entire EEPROM
  /*writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0xA0);
  writeEEPROM(0x0000, 0x00);
  writeEEPROM(0x0001, 0x00);//*/
  unsigned long time = micros();
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0x80);
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0x10);
  time = micros() - time;
  delay(20);
  Serial.println(time);
  Serial.print("Erasing EEPROM");
  for (uint16_t address = 0; address < 2047; address++) {
    writeEEPROM(address, 0xea);
    /*if (address % 0x0800 == 0){
      char output[5];
      sprintf(output, "%04x:", address);
      Serial.println("");
      Serial.print(output);
    }
    if (address % 0x0400 == 0){
      Serial.print("  ");
    }
    if (address % 0x40 == 0) {
      Serial.print(".");
    }//*/
  }
  Serial.println(" done");


  // Program data bytes
  Serial.print("Programming EEPROM");
  for (uint16_t address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);
    /*if (address % 64 == 0) {
      Serial.print(".");
    }//*/
  }
  Serial.println(" done");


  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // put your main code here, to run repeatedly:

}
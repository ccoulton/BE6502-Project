#include <Arduino.h>
//0->7
//portb ss/nc, sck, mosi, miso 8, 9, 10, 11
//portc nc, nc, nc, nc, nc, 5, 13
//portd 3, 2, 0, 1, 4, nc, 12, 6
//porte nc, nc, nc, nc, nc, 7, nc
//portf a5, a4, nc, nc, a3, a2, a1, a0
//SHIFT_DATA a5, SHIFT_CLK  a4 SHIFT_LATCH A3
//write_EN 12, output 3, chip_en 2
#define CONTROL_PORT    PORTD
#define CONTROL_DIR     DDRD
#define CONTROL_OUTPUT  0x4b
#define SHIFT_PORT      PORTF
#define SHIFT_DIR       DDRF
#define SHIFT_OUTPUT    0x13
#define LSB_PORT        PORTB
#define LSB_OUTPUT      0xfe
#define LSB_INPUT       0x01
#define MSB_PORT        PORTE
#define MSB_INPUT       0xbf
#define MSB_OUTPUT      0x40
#define EEPROM_D0 0
#define EEPROM_D7 7
int DATA_PIN[] = {SCK, MOSI, MISO, 8, 9, 10, 11, 7};
/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(uint16_t address) {

  for (int addressPin = 0; addressPin <= 14; addressPin++) {
    SHIFT_PORT =  0x01&address;
    SHIFT_PORT |= 0x02;//cycle clock high
    SHIFT_PORT &=~0x02;//cycle clock low
    address >>= 1;
  }
  SHIFT_PORT &= 0x10;
  SHIFT_PORT |= 0x10; //latch high
  SHIFT_PORT &=~0x10; //latch low
  delayMicroseconds(1);
  //shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8));
  //shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);
  //trigger the shift register to trigger all outputs at once rather than as they are clocked in.
  /*digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);*/
  //*/
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  DDRB = LSB_INPUT;
  DDRE = MSB_INPUT;
  CONTROL_PORT |= 0x40; //Write_en high
  CONTROL_PORT &=~0x03; //chip & output low
  setAddress(address);
  delayMicroseconds(2);
  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(DATA_PIN[pin]);
  }
  CONTROL_PORT |= 0x43;
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
//write_EN 12, output 3, chip_en 2
// 6            0           1
void writeEEPROM(uint16_t address, byte data) {
  DDRE = MSB_OUTPUT;
  DDRB = LSB_OUTPUT;
  setAddress(address);
  CONTROL_PORT |= 0x01;//output_ENABLE HIGH
  CONTROL_PORT &=~0x02;//Chip_ENABLE LOW
  CONTROL_PORT &=~0x40;//WRITE_EN LOW
  delayMicroseconds(1);
  MSB_PORT = data>>1;
  LSB_PORT = data<<1;
  delayMicroseconds(1);
  CONTROL_PORT |= 0x40;//WRITE_EN HIGH
  CONTROL_PORT |= 0x02;//CHIP_EN HIGH
  CONTROL_PORT &=~0x01;//OUTPUT_EN LOW
  delayMicroseconds(1);
  //digitalWrite(WRITE_EN, LOW);
  //delayMicroseconds(1);
  //digitalWrite(WRITE_EN, HIGH);s
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
  CONTROL_PORT |= 0x43;
  CONTROL_DIR = CONTROL_OUTPUT;
  SHIFT_DIR = SHIFT_OUTPUT;
  DDRE = MSB_OUTPUT; //0b01000000
  DDRB = LSB_OUTPUT; //0b11111110
  Serial.begin(57600);
  while(!Serial){;}
  // Erase entire EEPROM
  unsigned long time = micros();
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0x80);
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0x20);
  time = micros() - time;
  delay(20);
  Serial.println(time);
  Serial.println(readEEPROM(0x2AAA),HEX);
  Serial.println(readEEPROM(0x5555),HEX);
  Serial.print("Erasing EEPROM");
  for (uint16_t address = 0; address < 0x7fff; address++) {
    writeEEPROM(address, 0xea);
    if (address % 0x0800 == 0){
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

  setAddress(0);
  // Program data bytes
  Serial.print("Programming EEPROM");
  for (uint16_t address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);
    //while(!Serial.available()){;}
    //Serial.read();
    if (address % 64 == 0) {
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

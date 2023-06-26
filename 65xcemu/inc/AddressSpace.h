#include <fstream>
class AddressSpace {
  public:
      inline AddressSpace() { InitMemory(0, NULL); };
      
      inline AddressSpace(uint16 startingAddress) {
          InitMemory(startingAddress, NULL);
      };

      inline AddressSpace(std::ifstream *file, uint16 startingAddress) {
        InitMemory(startingAddress, file);
      };

      inline void loadTest() {
        for(int index=0; index<16; index++) {
            mem[0x8000+index] = testfile[index];
        }
        mem[0xfffc] = 0x00;
        mem[0xfffd] = 0x80;
      };

      inline uint8 fetchAddress(uint16 address) {
          return mem[address];
      };
  private:
    inline void InitMemory(uint16 startingAddress, std::ifstream *file) {
        for (uint16 index = startingAddress; index < 0xFFFF; index++) {
            uint8 nextbyte;
            if ((file != NULL) && (file->peek() != EOF)) {
                nextbyte = file->get();
            } else {
                nextbyte = 0xEA;
            }
            mem[index] = nextbyte;
        }
    };
    uint8 testfile[15] = {
        0xa9, 0xff,         //LDA #$ff
        0x8d, 0x02, 0x60,   //STA $6002
        0xa9, 0x55,         //LDA #$55
        0x8d, 0x00, 0x60,   //STA $6000
        0x49, 0xff,         //xor #$FF
        0x4c, 0x07, 0x80};  //JMP $8007
    uint8 mem[0xffff];
};

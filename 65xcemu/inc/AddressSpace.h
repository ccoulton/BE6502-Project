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

      inline uint8 fetchAddress(uint16 address) {
          return mem[address];
      };
  private:
    uint8 mem[0xffff];
};

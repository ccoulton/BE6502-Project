class AddressSpace {
  public:
      inline AddressSpace() { AddressSpace((uint16)0); };
      inline AddressSpace(uint16 startingAddress) {
          InitMemory(startingAddress);
      };
      inline void InitMemory(uint16 startingAddress) {
          for (int index = startingAddress; index < 0xFFFF; index++)
              mem[index] = 0xEA;
      };
      //inline AddressSpace(filepointer)
      inline uint8 fetchAddress(uint16 address) {
          return mem[address];
      };
  private:
    uint8 mem[0xffff];
};

class CPU {
    public:
        CPU(AddressSpace &program);
        void Reset(int *cycle);
        void run(int clock);
    private:
        void StoreAcc(int* cycle);
        void LoadRegister(uint8* outputRegister, int* clock);
        uint16 getZPOperandAddress(int* clock);
        //Vectors
        uint8 fetchValueFromAddress(uint16 address, int* cycle);
        void fetchWordintoRegister(uint16 address,
                                  Register* output,
                                  int* cycle);
        uint8 A;        //Accumulator
        Register PC;    //ProgramCounter
        Status PS;      //Processor status register
        uint8 SP;       //Stack pointer
        uint8 X;        //X Register
        uint8 Y;        //Y Register
        instructionRegister intReg;
        AddressSpace *mem;
};

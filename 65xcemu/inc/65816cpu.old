class CPU {
    public:
        CPU(AddressSpace &program);
        void Reset(int *cycle);
        void run(int clock);
    private:
        //Vectors
        uint8 fetchValueFromAddress(uint16 address, int* cycle);
        void fetchWordintoRegister(uint16 address,
                                  Register* output,
                                  int* cycle);
        Register A;     //Accumulator
        uint8 DBR;      //Data Bank register
        Register DIR;   //Direct
        uint8 K;        //Program Bank
        Register PC;    //ProgramCounter
        Status PS;      //Processor status register
        Register SP;    //Stack pointer
        Register X;     //X Register
        Register Y;     //Y Register
        bool emulation;
        instructionRegister intReg;
        AddressSpace *mem;
};

typedef unsigned char uint8;
typedef unsigned int uint16;

enum CPUVectors {
  COP   = 0xFFF4,
  ABORT = 0xFFF8,
  NMI   = 0xFFFA,
  RST   = 0xFFFC,
  IRQBRK= 0xFFFE
};

enum groups {
    GROUP3, GROUP1, GROUP2, GROUP4
};

/// aaabbbcc aaa & cc optcode, bbb addressing
enum groupXOpt { //aaaxxxcc
    //Group1 = 01
    ORA = 0, AND = 1, EOR = 2, ADC = 3,
    STA = 4, LDA = 5, CMP = 6, SBC = 7,
    //Group2 = 10
    ASL = 0, ROL = 1, LSR = 2, ROR = 3,
    STX = 4, LDX = 5, DEC = 6, INC = 7,
    //Group3 = 00
    BRK = 0, BIT = 1, JMP = 2, JMPABS = 3,
    STY = 4, LDY = 5, CPY = 6, CPX = 7,
    //Group4 = 11 65c816 only
    OFFS = 0,   DIRPAGE = 1,    ABSLONG = 3,
    OFFSYI = 4, DIRPAGEYI = 5,  ABSLONGXI = 7
};

enum groupAddressing { //xxxbbbxx
    ZPX   = 0, //g1 (zp, x)
    ZP    = 1, //gx zp
    IMD   = 2, //g1 #imd
    ABS   = 3, //gx absolute
    ZPYI  = 4, //gx (zp), y
    ZPXI  = 5, //gx zp, x
    ABSYI = 6, //absolute, y
    ABSXI = 7, //absolute, x
    IMMD  = 0, //g2 imd
    ACC   = 2, //g2 acc
    ZPXYI = 5, //g2 zero page x/y
    ABSXYI= 7  //g2 absolute x/y
};

//conditional branch xxy10000
//group3 Zero page y index
enum condBranchFlag { //xx flag compared to y
    negative,
    overflow,
    carry,
    zero
};

enum subroutinefunctions {
    JSRABS = 0x20, //b001 000 00 group3 immd bit
    RTI    = 0x40, //b010 000 00 group3 immd jmp
    RTS    = 0x60, //b011 000 00 group3 immd jmpABS
};

struct statusFlags {
    //Set if there was arithmetic carry. Or Emulation flag is swapped in
    bool ce : 1; //Carry                 0
    //Set if result is zero.
    bool z  : 1; //Zero                  1
    //If set Hardware IRQ is disabled.
    bool i  : 1; //IRQ                   2
    //If set ADC/SBC do BCD
    bool d  : 1; //Decimal               3
    //Set when x/y is 8 bits x or b for BRK (software)/IRQ (Hardware)
    bool xb : 1; //Index or break        4 (Native)/(Emulation)
    //Set when is acc and mem is 8b
    bool m  : 1; //Acc and mem Width     5 (Native)
    //Set when Arithmetic overflow
    bool v  : 1; //Overflow              6
    //Is set when result is negative
    bool n  : 1; //Negative              7
};

union Status {
    statusFlags flags;
    uint8 reg;
};

struct LOWHIGH {
    uint8 l;
    uint8 h;
};

union Register {
    LOWHIGH bytes;
    uint16 r;
};

struct groupXOptCode {
    uint8 group : 2;
    uint8 addressing : 3;
    uint8 optcode : 3;
};

struct condBranchCode {
    uint8 id  : 5; //0x10
    bool y    : 1;
    uint8 x   : 2;
};

union instructionRegister {
    uint8 reg;
    groupXOptCode groupx;
    condBranchCode cond;
};

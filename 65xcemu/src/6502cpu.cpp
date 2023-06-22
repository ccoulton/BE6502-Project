#include "../inc/typedef.h"
#include "../inc/AddressSpace.h"
#include "../inc/6502cpu.h"

CPU::CPU(AddressSpace &program) {
    A.r = 0;
    DBR = 0;
    DIR.r = 0;
    K = 0;
    PS.reg = 0x30;
    SP.r = 0;
    X.r = 0;
    Y.r = 0;
    emulation = true;
    intReg.reg = 0;
    mem = &program;
}

void CPU::Reset(int *cycle) {
    fetchWordintoRegister(RST, &PC, cycle);
}

uint8 CPU::fetchValueFromAddress(uint16 address, int* cycle) {
    (*cycle)--;
    return mem->fetchAddress(address);
}

void CPU::fetchWordintoRegister(uint16 address, Register* output, int* cycle) {
    output->bytes.l = fetchValueFromAddress(address++, cycle);
    output->bytes.h = fetchValueFromAddress(address, cycle);
}

void checkpageboundry(uint16 prev, uint16 curr, int* cycle) {
    (*cycle)--;
}

void CPU::run(int clock) {
    auto getcondflag = [this](const int value) {
        return  (value == negative) ? PS.flags.n  :
                (value == overflow) ? PS.flags.v  :
                (value == carry)    ? PS.flags.ce :
                                      PS.flags.z;
    };
    auto processBranchCond = [this](int *clock) {
    /* Handles conditional branching
     * BC(C|S), BNE, BEQ, BPL, BMI, BV(C|S)
     */
        if (intReg.cond.id == 0x10) {
            if (getcondflag(intReg.cond.x) != intReg.cond.y){
              PC.r += 2;
              (*clock)--;
            } else {
              fetchWordintoRegister(PC.r, &PC, clock);
              //check page boundry.
            }
            return true;
        }
        return false;
    };
    auto processNOP = [this](int *clock) {
        if ((intReg.reg == 0xEA) || (intReg.reg == 0x42)) {
          PC.r += (intReg.reg == 0x42); //WDM skips a byte
          (*clock)--;
          return true;
        }
        return false;
    };
    Reset(&clock);
    while (clock > 0) {
        intReg.reg = fetchValueFromAddress(PC.r++, &clock);
        if (processBranchCond(&clock)) {
            continue;
        }
        if (proccessNOP(&clock)) {
            continue;
        }
        if (intReg.reg == 0x40) { //return from Interupt
        }
        if (intReg.reg == 0x60) { //return from subroutine
        }
        if        (intReg.groupx.group == GROUP1) {
            switch (intReg.groupx.optcode) {
                case ORA:
                case AND:
                case EOR:
                case ADC:
                case STA:
                case LDA:
                case CMP:
                case SBC:
            }
        } else if (intReg.groupx.group == GROUP2) {
            switch (intReg.groupx.optcode) {
                case ASL:
                case ROL:
                case LSR:
                case ROR:
                case STX:
                case LDX:
                case DEC:
                case INC:
            }
        } else if (intReg.groupx.group == GROUP3) {
            //implied single byte.
        } else if (intReg.groupx.group == GROUP4) {

        }
        uint8 lownibble = intReg.reg & 0x0f;
        if (lownibble == 0x08) {
          if ((intReg.reg.addressing == ACC) &&
             (intReg.reg.optcode <= JMPABS)) {
              uint8 pointer = SP.r;
              uint8 *reg;
              if (intReg.reg.optcode <= BIT) {
                  reg = &PS.reg;
              //000 010 00  010 010 00
              //php/plp push/pull status <-> stack
              } else {
              //001 010 00  011 010 00
              //pha/pla push/pull acc <-> stack
                  reg = &A.bytes.l;
              }
              if (intReg.reg.optcode & 2) { //plx
                  //stack->reg
              } else { //phx
                  //reg->stack
              }
          } else if ((intReg.reg.addressing == ABS) &&
              (intReg.reg.optcode != STY)) {
              bool clearset = intReg.reg.optcode & 1;
              uint8 mask = (intReg.reg.optcode & 6);
              bool* flagptr;
              switch (mask) {
                case 0:
                  flagptr = &PS.flags.ce;
                  break;
                case 2:
                  flagptr = &PS.flags.i;
                  break;
                case 6:
                  flagptr = &PS.flags.d;
                  break;
                default:
                  flagptr = &PS.flags.v;
                  clearset = false;
                  break;
              }
              *flagptr = clearset;
          } else {
              uint8* pointer = &Y.bytes.l;
              //dey 100 010 00
              //dey dec y reg 8
              //iny 110 010 00 inx 111 010 00
              //iny/inx incerment y/x C/E
              //tya 100 110 00 tay 101 010 00
              //tya/tay transfer y <-> acc 9/A
          }
        } else if (lownibble == 0x0B) {
          //phd/pld push/pull Direct
          //phk push K
          //phb/plb push/pull DBR
          //tsc transfer stack->Acc(16)
          //tcd/tdc transfer ACC(16)<->Direct
          //xce swap c with emulation.
          //xba swap high and low byte in acc
          //txy/tyx copy x<->y
          //rtl/wai/stp
        } else if (lownibble == 0x0A) {
            if (intReg.reg >= 0x7A) {
                //ply, txa, txs, tax, tsx dex, phx, plx
            } else if (intReg.reg == 0x5A) { //phy
            }
        }
    }
    return;
}

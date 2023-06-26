#include "../inc/typedef.h"
#include "../inc/AddressSpace.h"
#include "../inc/6502cpu.h"

CPU::CPU(AddressSpace &program) {
    A = 0;
    PS.reg = 0x30;
    SP = 0;
    X = 0;
    Y = 0;
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

void CPU::StoreAcc(int* cycle) {
    Register tempReg;
    if (intReg.groupx.addressing == ABS) {
        fetchWordintoRegister(PC.r, &tempReg, cycle);
        PC.r += 2;
    }
    mem->writeAddress(tempReg.r, A);
}

uint16 CPU::getZPOperandAddress(int* clock) {
    uint16 operandAddress = 0;
    uint8 zpAddress = 0;
    Register temp;
    zpAddress += fetchValueFromAddress(PC.r++, clock);
    operandAddress = zpAddress;
    if ((intReg.groupx.addressing == ZPX) ||
        (intReg.groupx.addressing == ZPYI)) {
        zpAddress += (intReg.groupx.addressing == ZPX) ? X : 0;
        fetchWordintoRegister(zpAddress, &temp, clock);
        operandAddress = temp.r;
        operandAddress += (intReg.groupx.addressing == ZPYI) ? Y : 0;
    }
    return operandAddress;
}
 
void CPU::LoadRegister(uint8* outputRegister, int* clock) {
    uint16 operandAddress = 0;
    Register tempRegister;
    if (        (intReg.groupx.addressing == ZP)   ||
                (intReg.groupx.addressing == ZPX)  ||
                (intReg.groupx.addressing == ZPYI) || 
                (intReg.groupx.addressing == ZPXYI)) {
        operandAddress = getZPOperandAddress(clock);
        if (intReg.groupx.addressing == ZPXYI) {
            operandAddress += (&X == outputRegister)? Y : X;
        }
    } else if ( (intReg.groupx.addressing == ABS) ||
                (intReg.groupx.addressing == ABSYI) ||
                (intReg.groupx.addressing == ABSXI)) {
        fetchWordintoRegister(PC.r, &tempRegister, clock);
        PC.r += 2;
        operandAddress = tempRegister.r;
        if (intReg.groupx.addressing == ABSXI) {
            operandAddress += X ;
        } else if (intReg.groupx.addressing == ABSYI) {
            operandAddress += Y;
        }
    } else {//if (intReg.groupx.addressing == IMD)
        operandAddress = PC.r++;
    }
    *outputRegister = fetchValueFromAddress(operandAddress, clock);
    PS.flags.z = (*outputRegister == 0);
    PS.flags.n = (*outputRegister & 0x80);
}

void CPU::run(int clock) {
    auto processBranchCond = [this](int *clock) {
    /* Handles conditional branching
     * BC(C|S), BNE, BEQ, BPL, BMI, BV(C|S)
     */
        if (intReg.cond.id == 0x10) {
            bool condFlag = (intReg.cond.x == negative) ? PS.flags.n :
                            (intReg.cond.x == overflow) ? PS.flags.v :
                            (intReg.cond.x == carry)    ? PS.flags.ce:
                                                          PS.flags.z;
            if (condFlag != intReg.cond.y){
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
    auto processNOP = [this] (int *clock) {
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
        if (processNOP(&clock)) {
            continue;
        }
        if (intReg.reg == 0x40) { //return from Interrupt
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
                    if (intReg.groupx.addressing != IMD) {
                        StoreAcc(&clock);
                    } else {
                        // BIT #
                    }
                    break;
                case LDA:
                    LoadRegister(&A, &clock);
                    break;
                case CMP:
                case SBC:
                break;
            };
        } else if (intReg.groupx.group == GROUP2) {
            switch (intReg.groupx.optcode) {
                case ASL:
                case ROL:
                case LSR:
                case ROR:
                case STX:
                case LDX:
                    //LoadRegister(&X, &clock);
                    break;
                case DEC:
                case INC:
                break;
            };
        } else if (intReg.groupx.group == GROUP3) {
            //implied single byte.
            if (intReg.groupx.optcode == JMP){
                fetchWordintoRegister(PC.r, &PC, &clock);
            }
        } else if (intReg.groupx.group == GROUP4) {

        }
        uint8 lownibble = intReg.reg & 0x0f;
        if (lownibble == 0x08) {
          if ((intReg.groupx.addressing == ACC) &&
             (intReg.groupx.optcode <= JMPABS)) {
              uint8 pointer = SP;
              uint8 *reg;
              if (intReg.groupx.optcode <= BIT) {
                  reg = &PS.reg;
              //000 010 00  010 010 00
              //php/plp push/pull status <-> stack
              } else {
              //001 010 00  011 010 00
              //pha/pla push/pull acc <-> stack
                  reg = &A;
              }
              if (intReg.groupx.optcode & 2) { //plx
                  //stack->reg
              } else { //phx
                  //reg->stack
              }
          } else if ((intReg.groupx.addressing == ABS) &&
              (intReg.groupx.optcode != STY)) {
              bool clearset = intReg.groupx.optcode & 1;
              uint8 mask = (intReg.groupx.optcode & 6);
              switch (mask) {
                case 0:
                  PS.flags.ce = clearset;
                  break;
                case 2:
                  PS.flags.i = clearset;
                  break;
                case 6:
                  PS.flags.d = clearset;
                  break;
                default:
                  PS.flags.v = false;
                  break;
              }
          } else {
              uint8* pointer = &Y;
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

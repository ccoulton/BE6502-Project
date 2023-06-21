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

void CPU::run(int clock) {
  Reset(&clock);
  while (clock > 0) {
      intReg.reg = fetchValueFromAddress(PC.r++, &clock);
      uint8 lownibble = intReg.reg & 0x0f;
      if (lownibble == 0x08) {
        //php/clc/plp/sec/pha/cli/pla/sei/dey/tya/tay/clv/iny/cld/inx/sed
      } else if (lownibble == 0x0B) {
        //phd/tcs/pld/tsc/phk/tcd/rtl/tdc/phb/txy/plb/tyx/wai/stp/xba/xce
      } else if (lownibble == 0x0A) {
          if (intReg.reg >= 0x7A) {
              //ply, txa, txs, tax, tsx dex, phx, plx
              if (intReg.reg == 0xEA) { //inc:111 acc:010, group2:10
                  clock--; //nop
              }
              continue;
          } else if (intReg.reg == 0x5A) { //phy
          }
      } //0x42 wdm, 0x40 rti, 0x60 rts
  }
  return;
}

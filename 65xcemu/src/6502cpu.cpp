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
    Reset(&clock);
    while (clock > 0) {
        intReg.reg = fetchValueFromAddress(PC.r++, &clock);
        if (intReg.cond.id == 0x10) {
            if (getcondflag(intReg.cond.x) != intReg.cond.y) {
                PC.r += 2;
                clock--;
            } else {
                fetchWordintoRegister(PC.r, &PC, &clock);
                //if page boundry clock--;
            }
            continue;
        } 
        uint8 lownibble = intReg.reg & 0x0f;
        if (lownibble == 0x08) {
            //php/plp push/pull status <-> stack
            //clc/sec clear/set carry
            //pha/pla push/pull acc <-> stack
            //cli/sei clear/set i flag
            //dey dec y reg
            //tya/tay transfer y <-> acc
            //clv clear v
            //iny/inx incerment y/x 
            //cld/sed clear/set d
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

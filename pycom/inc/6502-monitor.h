#ifndef _6502_MONITOR_H_
#define _6502_MONITOR_H_

#define CLOCK 25 //interupt pin though optocouple

const char opcodeMatrix[256][5] = {\
        "BRK", "ORA", "COP", "ORA", "TSB", "ORA", "ASL", "RMB0", "PHP", "ORA", "ASL", "PHD", "TSB", "ORA", "ASL", "BBR0",\
        "BPL", "ORA", "ORA", "ORA", "TRB", "ORA", "ASL", "RMB1", "CLC", "ORA", "INC", "TCS", "TRB", "ORA", "ASL", "BBR1",\
        "JSR", "AND", "JSL", "AND", "BIT", "AND", "ROL", "RMB2", "PLP", "AND", "ROL", "PLD", "BIT", "AND", "ROL", "BBR2",\
        "BMI", "AND", "AND", "AND", "BIT", "AND", "ROL", "RMB3", "SEC", "AND", "DEC", "TSC", "BIT", "AND", "ROL", "BBR3",\
        "RTI", "EOR", "WDM", "EOR", "MVP", "EOR", "LSR", "RMB4", "PHA", "EOR", "LSR", "PHK", "JMP", "EOR", "LSR", "BBR4",\
        "BVC", "EOR", "EOR", "EOR", "MVN", "EOR", "LSR", "RMB5", "CLI", "EOR", "PHY", "TCD", "JMP", "EOR", "LSR", "BBR5",\
        "RTS", "ADC", "PER", "ADC", "STZ", "ADC", "ROR", "RMB6", "PLA", "ADC", "ROR", "RTL", "JMP", "ADC", "ROR", "BBR6",\
        "BVS", "ADC", "ADC", "ADC", "STZ", "ADC", "ROR", "RMB7", "SEI", "ADC", "PLY", "TDC", "JMP", "ADC", "ROR", "BBR7",\
        "BRA", "STA", "BRL", "STA", "STY", "STA", "STX", "SMB0", "DEY", "BIT", "TXA", "PHB", "STY", "STA", "STX", "BBS0",\
        "BCC", "STA", "STA", "STA", "STY", "STA", "STX", "SMB1", "TYA", "STA", "TXS", "TXY", "STZ", "STA", "STZ", "BBS1",\
        "LDY", "LDA", "LDX", "LDA", "LDY", "LDA", "LDX", "SMB2", "TAY", "LDA", "TAX", "PLB", "LDY", "LDA", "LDX", "BBS2",\
        "BCS", "LDA", "LDA", "LDA", "LDY", "LDA", "LDX", "SMB3", "CLV", "LDA", "TSX", "TYX", "LDY", "LDA", "LDX", "BBS3",\
        "CPY", "CMP", "REP", "CMP", "CPY", "CMP", "DEC", "SMB4", "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "BBS4",\
        "BNE", "CMP", "CMP", "CMP", "PEI", "CMP", "DEC", "SMB5", "CLD", "CMP", "PHX", "STP", "JML", "CMP", "DEC", "BBS5",\
        "CPX", "SBC", "SEP", "SBC", "CPX", "SBC", "INC", "SMB6", "INX", "SBC", "NOP", "XBA", "CPX", "SBC", "INC", "BBS6",\
        "BEQ", "SBC", "SBC", "SBC", "PEA", "SBC", "INC", "SMB7", "SED", "SBC", "PLX", "XCE", "JSR", "SBC", "INC", "BBS7"};
        
void onClock();
void DebuggerTask(void* parameters);

#endif
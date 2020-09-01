;============================================================================================

;RAM: 32k $0000 to $3FFF
;ROM  32k  $8000 to $FFFF
;LCD $8000 - $800F
;6522 $6000 - $600F
;
;==============================================================================

.target "65c02"
.setting "LaunchCommand", "wsl ../x6502/x6502 {0}"
.memory "fill", $8000, $8000, $ea
;==============================================================================
;Address values
RESET_VECTOR = $8000
IRQ_VECTOR = $8500

PB   = $6000    ; ORA / IRA
PA   = $6001    ; ORB / IRB
DDRB = $6002     ; DDRB
DDRA = $6003     ; DDRA
T1CL = $6004     ; T1 low order latches / counter
T1CH = $6005     ; T1 high order counter
T1LL = $6006     ; T1 low order latches
T1LH = $6007     ; T1 high order latches
T2CL = $6008     ; T2 low order latches / counter
T2CH = $6009     ; T2 high order counter
VIAA = $600A     ; shift register
ACR  = $600B     ; auxiliary control register
PCR  = $600C     ; peripheral control register
IFR  = $600D     ; interrupt flag register
IER  = $600E     ; interrupt enable register
VIAF = $600F     ; ORA/IRA without handshake

;==============================================================================
; ZERO PAGE

;==============================================================================
; MACROS

;==============================================================================
; SETUP
         .org $8000
reset:
; *** SETUP
         SEI               ; interrupts off
         LDX      #$FF     ; initialise stack
         TXS
         JSR      VIAINIT
         LDA      #$15     ; load low latch with decimal 21
         STA      T1LL
         STZ      T1CH     ; start timer by setting high byte (in this case to zero)
                           ; T1 will now count down to zero when the interrupt flag will be set
                           ; the counter is reloaded automatically in this mode, taking 2 clock cycles
                           ; the time of a complete reset and countdown is therefore n+2 cycles
         CLD
         CLI

;==============================================================================
; MAIN PROGRAM GOES HERE
;
LOOP     NOP
         JMP      LOOP
;==============================================================================

;SUBROUTINES
;------------------------------------------------------------------------------
; *** Via setup
VIAINIT  LDA      #%11111111        ; set all PB to output
         STA      DDRB              ; with ACR set as below PB7 will toggle each time
                                    ; T1 reaches zero

         LDA      #%11000000        ; ACR -  bit 7 set enables output pulses on PB7
                                    ;        bit 6 set put T1 into continuous mode
         STA      ACR

         LDA      #%11000000        ; interrupts - enable T1
         STA      IER
         LDA      #%00111111        ; interrupts - disable all except T1
         STA      IER
         RTS


;==============================================================================
;ISR
.org IRQ_VECTOR
         ; calling the ISR takes 7 clock cycles, then 4 more to carry out the next instruction
         ; total 11 cycles to reset the IFR in the 6522
         LDA      T1CL     ; clear IRQ flag by reading the low by byte of T1
         RTI

.org $fffc
.word $8000
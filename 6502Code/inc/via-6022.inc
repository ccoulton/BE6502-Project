; Via register constants
;VIAADDR = $6000

PORTB   = VIAADDR       ;data port for portb
PORTA   = VIAADDR + 1   ;data port for porta
DDRB    = VIAADDR + 2   ;Data Direction Register
DDRA    = VIAADDR + 3   ;dito
TMR1CL  = VIAADDR + 4   ;timer one low order latches/counter
TMR1CH  = VIAADDR + 5   ;timer one high order counter
TMR1LL  = VIAADDR + 6   ;timer one low order latches
TMR1LH  = VIAADDR + 7   ;timer one high order latches
TMR2CL  = VIAADDR + 8   ;timer two low order latches/cnt
TMR2CH  = VIAADDR + 9   ;timer two high order counter
SHFTREG = VIAADDR + 10  ;shift register
AUXCTR  = VIAADDR + 11  ;Auxilary control Register
PERCTR  = VIAADDR + 12  ;Peripheral Control Register
VIAIRQF = VIAADDR + 13  ;interupt Flag Register
VIAIRQE = VIAADDR + 14  ;Interupt Enable Register
VIAORA  = VIAADDR + 15  ;Port A w/ no handshake

/* Peripheral control register values + $0C
 * Shift 4 bits down for CA2 or 1
 * ; cb2ctr 7-5 cb1 4 ca2 3-1 ca1 0
 */
 CB2IndIrqNeg = $20
 CB2InptAct   = $40
 CB2IndIrqPos = $60
 CB2HandShake = $80
 CB2PluseOut  = $A0
 CB2LowOutput = $C0 
 CB2HiOutput  = $e0    
 CB1PosActEdg = $10 ; Positive Active Edge

/* Auxiliary Control Register Format and Operation + $0B
 * ; 76 t1 ctr 5 t2 4-2 shft, pb, pa
 */
 TMR1CNTNPB7  = $C0 ; Timer 1 control continuous interupts, square
 TMR1ONESHOT  = $80 ; Timed IRQ each time counter/latch is loaded 1 shot
 TMR1CNTPB7DB = $40 ; timer 1 continuous but pb7 is not triggered
 TMR2COUNTPB6 = $20 ; and not for timed interupt,
; SHIFT in registers control
 SRT2CLK      = $04
 SRPHICLK     = $08 
 SREXCLK      = $0C
; shift out and with shift in
 SHOUT        = $10
 LTCHENABLEPB = $02
 LTCHENABLEPA = $01

/* IRQ Flags $0D
 */
 ANYIRQ = $80   ;clear all irq 
 T1OUT  = $40   ;read t1c-l or write t1l-h
 T2OUT  = $20   ;read t2 l or write t2 h
 CB1ACT = $10   ;read or write orb
 CB2ACT = $08   ;read or write orb or ifr if indpendant
 SReg8  = $04   ; after 8 shifts 
 CA1ACT = $02   ;read from ORA
 CA2ACT = $01   ;read for Ora or ifr if indpendant

/* IRQ Enable $0E
 */
 STCLR   = $80
 T1IRQ   = $40
 T2IRQ   = $20
 CB1IRQ  = $10
 CB2IRQ  = $08
 SRIRQ   = $04
 CA1IRQ  = $02
 CA2IRQ  = $01

.macro via_init(PortADir=$ff, PortBDir=$ff)
    lda #PortADir
    sta DDRA
    lda #PortBDir
    sta DDRB
.endmacro

/**
* Macros and Address Defines for WDC 6551.
* Assembled with RetroAssembler.
**/

ACIA_ADDR = $5000
ACIA_DATA = ACIA_ADDR
ACIA_STAT = ACIA_ADDR+1 ;read only
ACIA_CMD  = ACIA_ADDR+2
ACIA_CNTL = ACIA_ADDR+3

/* Status Registers */
STAT_IRQ    = $80
STAT_DSRB   = $40
STAT_DCDB   = $20
STAT_TDRE   = $10
STAT_RXAV   = $08
STAT_OVRN   = $04
STAT_FRAM   = $02
STAT_PRTY   = $01

/* Control Registers
 *  Defaults 1 bit stop bit, and 8 word length, external rx clock
 */
SBN_2BIT    = $80 /*or 1.5 at wl=5 &! parity, and 1 for wl= 8 & parity*/ 
WD_7LGTH    = $20
WD_6LGTH    = $40
WD_5LGTH    = $60
RXCLKSRC    = $10 ; Set Baud rate with low Nibble
/* baud rates go from 16x-19200  key ones are 9600, or 19,200 */
SBR_19200   = $0F
SBR_9600    = $0E

/* Command Register 
 *Defaults Odd Parity, parity disabled, rx echo, txirq disabled, Rx irq Enabled, DTR NR
 */
PARITY_EVEN = $40
PARITY_MARK = $80
PARITY_SPC  = $C0
PME_ENABLE  = $20
REM_ECHO    = $10
TX_IRQ_ENB  = $04 ;rtsb = low tx irq
TX_IRQ_DSB  = $08
TX_BRK_TXD  = $0C
RX_IRQ_DSB  = $02
DTR_TML_RDY = $01

.macro acia_init(Command=$0B, Control=$1F)
    lda #Command        ; default no Parity, no echo, no irq
    sta ACIA_CMD
    lda #Control        ; default 1 stop bit, 8 data bits 19200 baud
    sta ACIA_CNTL
.endm

/* Serial read macro, compares status register for rx avaliable flag */
.macro acia_read(ReadAvaiable = STAT_RXAV)
    @read_wait
    lda ACIA_STAT
    and #ReadAvaiable
    beq @read_wait      ;could use *-5 to jump back 5 bytes to skip label
    lda ACIA_DATA       ;but that make's it not as portable/readable
.endm

/* Transmit Acc on ACIA, and wait till it's done, Broken on WDC as TDRE is always high*/
.macro acia_write_sync()
    sta ACIA_DATA
    acia_txFin()
.endm

//todo: for wdc set up 6522 timer pin for wait.
.macro acia_txFin(TxFinished = STAT_TDRE)
    pha
    @write_wait
    lda ACIA_STAT
    and #TxFinished
    beq @write_wait
    pla
.endm
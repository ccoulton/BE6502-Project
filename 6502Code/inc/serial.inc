ACIA_ADDR = $5000
ACIA_DATA = ACIA_ADDR
ACIA_STAT = ACIA_ADDR+1 ;read only
ACIA_CMD  = ACIA_ADDR+2
ACIA_CNTL = ACIA_ADDR+3

.function acia_init()
    lda #$0B        ;no Parity, no echo, no irq
    sta ACIA_CMD
    lda #$1F        ; 1 stop bit, 8 data bits 19200 baud
    sta ACIA_CNTL
.endfunction

.function wait_char()
    lda ACIA_STAT
    and #$08
    beq wait_char
    lda ACIA_DATA
.endfunction
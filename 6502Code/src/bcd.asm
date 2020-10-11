.target "65816"
.setting "LaunchCommand", "wsl /mnt/c/Users/Owner/Documents/BE6502-Project/x6502/x6502 {0}"
.memory "fill", $8000, $8000, $ea
; Via register constants
PORTB   = $6000
PORTA   = $6001
DDRB    = $6002
DDRA    = $6003
T1CL    = $6004
T1CH    = $6005
T1LL    = $6006
T1LH    = $6007
T2CL    = $6008
T2CH    = $6009
SHFREG  = $600A
AUXCTR  = $600B ; 76 t1 ctr 5 t2 4-2 shft, pb, pa
PERCTR  = $600C ; cb2ctr 7-5 cb1 4 ca2 3-1 ca1 0
VIAIRQF = $600D
VIAIRQE = $600E

value = $0200 ; 2 bytes
mod10 = $0202 ; 2 bytes
message = $0204 ; 6 bytes

ACIA_DATA = $5000
ACIA_STAT = $5001 ;read only
ACIA_CMD  = $5002
ACIA_CNTL = $5003

LCD_EN  = $80
LCD_RW  = $40
LCD_RS  = $20

.org $8000

reset:
    ; set stack pointer
    ldx #$FF
    txs 
    ; set up acia
    lda #$0B ; no parity, no echo, no irq
    sta ACIA_CMD
    lda #$1F ; 1 stop bit, 8 data bits 19200 baud
    sta ACIA_CNTL
    ; set up via
    lda #$FF ; port b all outputs
    sta DDRB
    lda #$E0 ; port a top 3 outputs
    sta DDRA
    ; set up LCD
    lda #$38 ;set 8-bit mode 2line disp 5x8
    jsr lcd_inst
    lda #$0E ;LCD on; Cursor on; blink off
    jsr lcd_inst
    lda #$06 ; Inc and sht cursor; don't shift display
    jsr lcd_inst
    lda #$01 ; Clear display
    jsr lcd_inst

    lda #0
    sta message
    ; init value to convert.
    lda number
    sta value
    lda number + 1
    sta value + 1

divide:
    ; init bcd
    lda #0
    sta mod10
    sta mod10 + 1
    clc
    
    ldx #16
divloop:
    ; Rotate Quotient and remainder
    rol value
    rol value + 1
    rol mod10
    rol mod10 + 1

    ; a, y = dividend - divisor
    sec
    lda mod10
    sbc #10
    tay ; save low byte to y
    lda mod10 + 1
    sbc #0
    bcc ignore_result ; branch if div < divis
    sty mod10
    sta mod10 + 1

ignore_result:
    dex
    bne divloop
    rol value ; shift in last bit of quotient
    rol value + 1

    lda mod10
    clc
    adc #"0"
    jsr push_char

    ; if value != 0, then continue
    lda value
    ora value + 1
    bne divide ; branch if value not z

    ldx #0
print:
    lda message,x
    beq irqsetup
    jsr print_char
    inx
    jmp print

irqsetup:
    lda AUXCTR
    and #$7F
    ora #$40
    sta AUXCTR
    lda #$0E
    sta T1CL
    lda #$00
    sta T1CH
    lda #$C0
    sta VIAIRQE
    cli
waitloop:
    jmp waitloop

number: .word 1729

isr:
    bit T1CL ; clear irq source
    lda #$40
    sta VIAIRQE
    sei
    ;rti but is infinate loop so reset for example 
    jmp reset; jump to reset 

; add the char in a register to beginn
; null term string 'message'
push_char:
    pha
    ldy #0
    
char_loop:
    lda message,y ; get char on string and put into x
    tax 
    pla
    sta message,y
    iny 
    txa
    pha
    bne char_loop
    pla
    sta message,y ; pull the null off the stack and add to eom
    rts

lcd_wait:
    pha
    stz DDRB
lcdbusy:
    lda #LCD_RW
    sta PORTA
    ora #LCD_EN
    sta PORTA
    lda PORTB
    and #$80
    bne lcdbusy

    lda #LCD_RW
    sta PORTA
    lda #$ff
    sta DDRB
    pla
    rts

lcd_inst:
    jsr lcd_wait
    sta PORTB
    stz PORTA
    lda #LCD_EN
    sta PORTA
    stz PORTA
    rts

print_char:
    jsr lcd_wait
    sta PORTB
    lda #LCD_RS
    sta PORTA
    ora #LCD_EN
    sta PORTA
    and #LCD_RS
    sta PORTA
    rts

.org $ffe4
.word $0000 ;cop_isr_nat = 00ffe4
.word $0000 ;brk_isr_nat = 00ffe6
.word $0000 ;abt_isr_nat = 00ffe8
.word $0000 ;nmi_isr_nat = 00ffea
.word $eaea ;res         = 00ffec
.word $0000 ;irq_isr_nat = 00ffee 
.org  $fff4 ;irq vectors
.word $8000 ;cop_ISR_emu = 00fff4
.word $eaea ;reserved    = 00fff6
.word $8000 ;abt_ISR_emu = 00fff8
.word $8000 ;nmi_ISR_emu = 00fffa
.word reset ;rst         = 00fffc 
.word isr ;cnt_isr     = 00fffe
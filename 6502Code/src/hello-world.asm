.target "65816"
.setting "LaunchCommand", "wsl /mnt/c/Users/Owner/Documents/BE6502-Project/x6502/x6502 {0}"
.memory "fill", $8000, $8000, $ea
PORTB   = $6000
PORTA   = $6001
DDRB    = $6002
DDRA    = $6003

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

    ldx #0
printloop:
    lda message,x
    beq mainloop
    jsr print_char
    inx
    jmp printloop

mainloop:
    ldx #0
print_SER: 
    lda ACIA_STAT
    and #$10; wait ms txempty broken.
    beq print_SER
    lda message,x
    beq read_SER
    sta ACIA_DATA
    inx
    jmp print_SER
read_SER:
    lda ACIA_STAT
    and #$08
    beq read_SER
    lda ACIA_DATA
    jmp mainloop

message: .byte "Hello, World!", $0d, $0a, $00

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
;res = 00ffe0
;res = 00ffe2
.org $ffe4
.word 0000 ;cop_isr_nat = 00ffe4
.word 0000 ;brk_isr_nat = 00ffe6
.word 0000 ;abt_isr_nat = 00ffe8
.word 0000 ;nmi_isr_nat = 00ffea
.word $eaea;res         = 00ffec
.word 0000 ;irq_isr_nat = 00ffee 
;.org $fff0 ;irq vectors
.word $eaea ;reserved    = 00fff0
.word $eaea ;reserved    = 00fff2
.word 0000 ;cop_ISR_emu = 00fff4
.word $eaea ;reserved    = 00fff6
.word 0000 ;abt_ISR_emu = 00fff8
.word 0000 ;nmi_ISR_emu = 00fffa
.word reset;rst 00fffc 
.word 0000 ;cnt_isr 00fffe

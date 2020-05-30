.target "65816"
.setting "LaunchCommand", "wsl ../x6502/x6502 {0}"
.memory "fill", $8000, $8000, $ea
PORTB   = $6000
PORTA   = $6001
DDRB    = $6002
DDRA    = $6003

LCD_EN  = $80
LCD_RW  = $40
LCD_RS  = $20

.org $8000

reset:
    ldx #$FF
    txs 

    lda #$FF
    sta DDRB
    lda #$E0
    sta DDRA

    lda #$38
    jsr lcd_inst
    lda #$0E
    jsr lcd_inst
    lda #$06
    jsr lcd_inst
    lda #$01
    jsr lcd_inst

    ldx #0
printloop:
    lda message,x
    beq mainloop
    jsr print_char
    inx
    jmp printloop

mainloop:
    jmp mainloop

message: .asciiz "Hello, World!"

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
;cop_isr_nat = 00ffe4
;brk_isr_nat = 00ffe6
;abt_isr_nat = 00ffe8
;nmi_isr_nat = 00ffea
;res = 00ffec
;irq_isr_nat = 00ffee 
.org $fff0 ; //irq vectors
.word 0000 ;res
.word 0000 ;res
.word 0000 ;cop_ISR_emu = 00fff4
.word 0000 ;res
.word 0000 ;abt_ISR_emu = 00fff8
.word 0000 ;nmi_ISR_emu = 00fffa
.word reset; 00fffc 
.word 0000 ;cnt_isr 00fffe

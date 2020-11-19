.target "65816"
.memory "fill", $8000, $8000, $ea
ACIA_ADDR = $5000
VIAADDR   = $6000
LCD_EN  = $80
LCD_RW  = $40
LCD_RS  = $20

.org $8000

reset:
    ldx #$ff
    txs
    acia_init()
    via_init($E0)
    lcd_init()

    ldx #0
printloop:
    lda message,x
    beq main
    jsr print_char
    inx
    jmp printloop

main:
    ldx #0
@serial
    lda message,x
    acia_write_sync()
    inx
    jmp @serial
    acia_read()
    jmp main

message: .asciiz "Hello, World!\r\n"
.include "../inc/serial.inc"
.include "../inc/via-6022.inc"
.include "../inc/lcd.inc"
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
.word $8000 ;cnt_isr     = 00fffe
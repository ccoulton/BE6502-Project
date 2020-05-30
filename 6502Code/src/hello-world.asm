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
    ldx #$ff
    txs 

    lda #$ff
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
    lda #$00
    sta DDRB
lcdbusy:
    lda #LCD_RW
    sta PORTA
    lda #(LCD_RW | LCD_EN)
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
    lda #0
    sta PORTA
    lda #LCD_EN
    sta PORTA
    lda #0
    sta PORTA
    rts

print_char:
    jsr lcd_wait
    sta PORTB
    lda #LCD_RS
    sta PORTA
    lda #(LCD_RS | LCD_EN)
    sta PORTA
    lda #LCD_RS
    sta PORTA
    rts

.org $fffc
.word reset
.word $0000

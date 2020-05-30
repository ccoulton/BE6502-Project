.target "65C02"
.setting "LaunchCommand", "wsl ../x6502/x6502 {0}"
.memory "fill", $8000, $8000, $ea
PORTB   = $6000
PORTA   = $6001
DDRB    = $6002
DDRA    = $6003

LCD_EN  = $80
LCD_RW  = $40
LCD_RS = $20

.org $8000
reset:
lda #$ff
sta DDRB

lda #$e0
sta DDRA

lda #$38 ;set 8-bit mode 2line disp 5x8
sta PORTB
lda #0 ; clear rs/rw/e
sta PORTA
lda #LCD_EN 
sta PORTA
lda #0 ; clar rs/rw/e
sta PORTA

lda #$0E ;LCD on; Cursor on; blink off
sta PORTB
lda #0
sta PORTA
lda #LCD_EN
sta PORTA
lda #0
sta PORTA

lda #$06
sta PORTB
lda #0
sta PORTA
lda #LCD_EN
sta PORTA
lda #0
sta PORTA

lda #"H"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"e"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"l"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"l"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"o"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #","
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #" "
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"W"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"o"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"r"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"l"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"d"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

lda #"!"
sta PORTB
lda #LCD_RS
sta PORTA
lda #(LCD_RS | LCD_EN)
sta PORTA
lda #LCD_RS
sta PORTA

loop:
jmp loop

.org $fffc
.word reset
.word 0000
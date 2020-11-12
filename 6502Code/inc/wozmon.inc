;.setting "HandleLongBranch", true
.org $FE00
LASTOPENL   = $24
LASTOPENH   = $25
STOREADDRL  = $26
STOREADDRH  = $27
HEXPARSEL   = $28
HEXPARSEH   = $29
YSAVE       = $2A
WOZMODE     = $2B ;$00=LastOpen, $7f= store, $AE Block examine 
MSGL        = $2C
MSGH        = $2D
CONTER      = $2E
CRC         = $2F
CRCCHK      = $30

INBUF       = $0200
CTLR_6551   = $5003
DATA_6551   = $5000

BSKEY  .ascii "\b"   ;Backspace ascii/arrowleft?
RETURN .ascii "\r"   ;
ESCKEY      = $1b   
PROMPT .ascii "\\"

WozReset:
    cld             ;clear decimal
    cli
    acia_init()     ;init 6551
SoftReset:
    lda #$9b
    ldy #$7f    ;   auto esc
NotReturn:
    cmp #BSKEY
    beq Backspace
    cmp #ESCKEY
    beq Escape
    iny
    bpl NextChar
Escape:
    lda #PROMPT     ;print prompt char
    Echo()         ;call echo sub
GetLine:
    lda #RETURN     ;print newline
    Echo()         ;call display print
    lda #PROMPT
    Echo()
    ldy #$01        ;start new input line
Backspace:
    dey             ;move text input
    bmi GetLine
    lda #$A0
    Echo()
    lda #BSKEY
    Echo()
NextChar:
    wait_char()
    cmp #$60        ;is lowercase
    bmi NUC         ;skip conversion
    and #$5f        ;convert to uppercase
NUC:sta INBUF,y     ;store into inputbuffer
    Echo()          ;print it
    cmp #RETURN     
    bne NotReturn   ;get more input
;command finished Lets parse.
    ldy #$FF        ;reset text pointer
    lda #$00        ;default mode is examine
    tax
SetStore:
    asl             ;leaves $7b in stor?
SetMode:
    sta WOZMODE     ; set mode flags
BlockSkip:
    iny
NextItem:
    lda INBUF,y     ;get next char
    cmp #RETURN
    beq GetLine
    cmp #"."
    blt BlockSkip
    beq SetMode
    cmp #":"
    beq SetStore
    cmp #"R"
    beq RunCommand  ;run stored program
    ;cmp #$CC        ;#lcmd
    ;beq LOADINT
    stx HEXPARSEL   ;clear input values
    stx HEXPARSEH
    sty YSAVE       ;save y for compare
NextHex:
    lda INBUF,y
    xor #$B0
    cmp #9+1        ;compare for decimal y not 10?
    blt RotateHex
    adc #$88
    cmp #$FA
    blt NotHex
RotateHex:
    .loop 4
        asl             ;acc shift left 4
    .endloop
    ldx #4          ;number of shift
HexShift:
    asl
    rol HEXPARSEL
    rol HEXPARSEH
    dex
    bne HexShift    ;loop for next hex
    iny             ;advance text input
    bne NextHex     ;always taken jmp? size
NotHex:
    cpy YSAVE
    beq Escape
    bit WOZMODE
    bvc NotStore
;storemode
    lda HEXPARSEL
    sta (STOREADDRL,x)
    inc STOREADDRL
    bne NextItem
    inc STOREADDRH
NTI:jmp NextItem        ;label to fix long jump
RunCommand:
    jsr ActiveRun
    jmp SoftReset
    ;LOADINT:
    ;    jsr LoadIntel
    ;    jmp SoftReset
NotStore:
    bmi ExamineNext
    ldx #2
SetAddress:
    lda HEXPARSEL-1,x
    sta STOREADDRL-1,x
    sta LASTOPENL-1,x
    dex
    bne SetAddress  ;while x != 0
NextPrint:
    bne PrintData
    lda #RETURN
    Echo()
    lda LASTOPENH
    jsr PrintByte
    lda LASTOPENL
    jsr PrintByte
    lda #':'
    Echo()
PrintData:
    lda #" "
    Echo()
    lda (LASTOPENL,x)
    jsr PrintByte
ExamineNext:
    stx WOZMODE
    lda LASTOPENL
    cmp HEXPARSEL
    lda LASTOPENH
    sbc HEXPARSEH                  
    bge NTI             ;there be more data to output!#longjump
    inc LASTOPENL
    bne Modulo8Check    ;No Carry
    inc LASTOPENH
Modulo8Check:           ;linefeed check could check any value?
    lda LASTOPENL       ;if address mod 8 = 0 start new line
    and #$07
    bpl NextPrint       ;always taken jmp?
PrintByte:
    pha
    .loop 4
        lsr
    .endloop
    jsr PrintHex
    pla
PrintHex:
    and #$0f            ;mask LSB
    ora #"0"
    cmp #"9"+1          ;is decimal?
    blt Echo
    adc #6              ;add w/carry 6 to offset for a-f
.function Echo()        ;write to to the serial bus
    lda CTLR_6551       ;has b7 cleared ;maybe use this for the lcd
    and #$10
    beq Echo
    sta DATA_6551     
.endf                   ;return from sub
ActiveRun:
    jmp (LASTOPENL)
    
.include "serial.inc"
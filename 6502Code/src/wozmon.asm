.target "6502"

.org $ff00
LASTOPENL   .equ    $24
LASTOPENH   .equ    $25
STOREADDRL  .equ    $26
STOREADDRH  .equ    $27
HEXPARSEL   .equ    $28
HEXPARSEH   .equ    $29
YSAVE       .equ    $2A
WOZMODE     .equ    $2B ;$00=LastOpen, $7f= store, $AE Block examine 

INBUF       .equ     $0200

KEYIN       = $D010     ;PIA.A register
KEYCNTRLREG = $D011     ;PIA.A ctrl register
DISPLAYPORT = $D012     ;pia.b output reg
DSPLYCTRLREG= $D013     ;pia.b contrl reg

BSKEY       = $DF   ;Backspace ascii/arrowleft?
RETURN      = $8D   ;
ESCKEY      = $98   
PROMPT .ascii "\\"

WozReset:
    cld     ;clear decimal
    cli     ;clear interupt
    ldy #$7F
    sty DISPLAYPORT
    lda #$A7
    sta KEYCNTRLREG
    sta DSPLYCTRLREG
NotReturn:
    cmp BSKEY
    beq Backspace
    cmp ESCKEY
    beq Escape
    iny
    bpl NextChar
Escape:
    lda PROMPT     ;print prompt char
    Echo()        ;call echo sub
GetLine:
    lda RETURN     ;print newline
    Echo()        ;call display print
    ldy #0+1        ;start new input line
Backspace:
    dey             ;move text input
    bmi GetLine
NextChar:
    lda KEYCNTRLREG
    bpl NextChar
    lda KEYIN
    sta INBUF,y     ;store into inputbuffer
    Echo()          ;print it
    cmp RETURN     
    bne NotReturn   ;get more input
;command finished
    ldy #$ff        ;reset text pointer
    lda #0          ;default mode is examine
    tax
SetStore:
    asl             ;leaves $7b in stor?
SetMode:
    sta WOZMODE     ; set mode flags
BlockSkip:
    iny
NextItem:
    lda INBUF,y     ;get next char
    cmp RETURN
    beq GetLine
    cmp #"."
    blt BlockSkip   ; if its not . or a char ignore it.
    beq SetMode     ; char is . set the mode
    cmp #":"
    beq SetStore
    cmp #"R"
    beq RunCommand  ;run stored program
    stx HEXPARSEL   ;clear input values
    stx HEXPARSEH
    sty YSAVE       ;save y for compare
NextHex:
    lda INBUF,y
    xor #$B0
    cmp #9+1        ;compare for decimal y not 10?
    blt Dig
    adc #$88
    cmp #$FA
    blt NotHex
Dig:
    asl             ;acc shift left 4
    asl             ;rol 4
    asl
    asl
    ldx #4          ;number of shift
HexShift:
    asl
    rol HEXPARSEL
    rol HEXPARSEH
    dex
    bne HexShift    ;loop for next hex
    iny             ;advance text input
    bne NextHex     ;always taken jmp?
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
TNI:jmp NextItem        ;label to fix long jump
RunCommand:
    jmp LASTOPENL
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
    jsr Echo
    lda (LASTOPENL,x)
    jsr PrintByte
ExamineNext:
    stx WOZMODE
    lda LASTOPENL
    cmp HEXPARSEL
    lda LASTOPENH
    sbc HEXPARSEH
    bge TNI             ;there be more data to output!#longjump
    inc LASTOPENL
    bne Modulo8Check    ;No Carry
    inc LASTOPENH
Modulo8Check:           ;linefeed check could check any value?
    lda LASTOPENL       ;if address mod 8 = 0 start new line
    and #$07
    bpl NextPrint       ;always taken jmp?
PrintByte:
    pha
    lsr
    lsr
    lsr
    lsr
    jsr PrintHex
    pla
PrintHex:
    and #$0f            ;mask LSB
    ora #"0"
    cmp #"9"+1          ;is decimal?
    blt Echo
    adc #6              ;add w/carry 6 to offset for a-f
.function Echo()
    bit DISPLAYPORT     ;has b7 cleared ;maybe use this for the lcd
    bmi Echo
    sta DISPLAYPORT     
.endf                   ;return from sub
.org $fffa
NMI_VECT .word $0F00
RST_VECT .word WozReset
IRQ_VECT .word $0000
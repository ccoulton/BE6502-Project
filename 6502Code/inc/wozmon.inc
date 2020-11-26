;.setting "HandleLongBranch", true
.org $FE00
LASTOPENL   = $0024 ;xaml
LASTOPENH   = $0025 ;xaml high
STOREADDRL  = $0026
STOREADDRH  = $0027
HEXPARSEL   = $0028
HEXPARSEH   = $0029
YSAVE       = $002A
WOZMODE     = $002B ;$00=LastOpen, $7f= store, $AE Block examine 

INBUF       = $0200 ; input buffer $027f

BSKEY       = $08   ;Backspace ascii/arrowleft?
DELKEY      = $7f   ;delete key
RETURN      = $0D   ; \r
NEWLINE     = $0A   ; \n
ESCKEY      = $1b   ; escape char
PROMPT      = $5c   ; \ character
COLON       = $3A   ; : character
DOT         = $2E   ; . character
SPKEY       = $20   ; space character

WozReset:
    cld             ;clear decimal
    cli
    ;acia_init()     ;init 6551
SoftReset:
    lda #ESCKEY
    ldy #$7f        ;triggers next char jump @37
NotReturn:
    cmp #BSKEY
    beq Backspace
    cmp #ESCKEY
    beq Escape
    iny
    bpl NextChar    ;auto esc if y >127 $7f
Escape:
    lda #PROMPT     ;print prompt char
    Echo()          ;call echo sub
GetLine:
    lda #RETURN     ;print newline
    Echo()          ;call display print
    ;lda #PROMPT
    ;Echo()
    ldy #$01        ;start new input line
Backspace:
    dey             ;move text input
    bmi GetLine
    lda #SPKEY      ;space
    Echo()
    lda #BSKEY      ;delete char
    Echo()
NextChar:
    acia_read()
    ;cmp #$60        ;is lowercase
    ;bmi UC          ;skip conversion
    ;and #$5f        ;convert to uppercase
UC: sta INBUF,y     ;store into inputbuffer
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
    beq GetLine     ;finished
    cmp #DOT        ;"."
    blt BlockSkip
    beq SetMode
    cmp #COLON      ;":"
    beq SetStore
    cmp #$52        ;"R"
    beq RunCommand  ;run stored program
    ;cmp #$4C        ;#lcmd
    ;beq LOADINT
    stx HEXPARSEL   ;clear input values
    stx HEXPARSEH
    sty YSAVE       ;save y for compare
NextHex:
    lda INBUF,y
    xor #$60
    cmp #10        ;compare for decimal y not 10?
    blt RotateHex
    adc #$08
    cmp #$7A
    blt NotHex
RotateHex:
    .loop 4
        asl             ;acc shift left 4
    .endloop
    ldx #$04          ;number of shift
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
    jmp (LASTOPENL)
    ;jsr ActiveRun
    ;jmp SoftReset
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
    lda #COLON
    Echo()
PrintData:
    lda #SPKEY
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
    bne Mod8Check       ;No Carry
    inc LASTOPENH
Mod8Check:
    lda LASTOPENL       ;if address mod 8 = 0 start new line
    and #$0F
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
    ora #$30
    cmp #$3a          ;is decimal?
    blt Echo
    adc #6              ;add w/carry 6 to offset for a-f
.function Echo()        ;write to to the serial bus
    acia_write_sync()     
.endf                   ;return from sub
;ActiveRun:
;    jmp (LASTOPENL)
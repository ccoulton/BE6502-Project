/* Commands and registers for HD44780U LCD */

/* RS low instruction Register or busy flag in read
 * High data Registers
 */

/* R/~W selects read or write high, DB7 is status*/
/* E starts R/W transfer */

/*db4-7, db0-3 */
//high nibble, pulse e, low nibble pulse e, bring rw up
// bf ac6-4, ac3-0, dr7-4, dr3-0

//busy state ends after 10ms after vcc
CLRDISPLAY  = $01
RETHOME     = $02 ;       1.52 ms set ddram to 0, 
; followeing are 37 us
ENTRYMODE   = $04 ; inc/dec, S enable shift 37 us
DISPLAY     = $08 ; D entire display on off, c cursour and blink
CURSOUR     = $10 ; s/c display/cursor move right/left - -
FNCTSET     = $20 ; DL 8/4  N 2/1 lines F 5x10/5x8 - -
SETCGADDR   = $40 ; ACG*6
SETDDADDR   = $80 ; ADD*

ENTRYINC    = ENTRYMODE | $02
;ENTRYDEC   = ENTRYMODE
ENTRYSHIFT  = ENTRYMODE | $01

DISENABLE   = DISPLAY | $04
CORENABLE   = DISPLAY | $02
BLNKENABLE  = DISPLAY | $01
//default is 4 bit, 1 line 5x8
FNCT8Bit    = FNCTSET | $10
;FNCT4Bit    = FNCTSET
FNCT2Line   = FNCTSET | $08
;FNCT1Line   = FNCTSET
FNCT5by10   = FNCTSET | $04
;FNCT5by8   = FNCTSET

.function lcd_wait()
    pha
    stz DDRB
@lcdbusy
    lda #LCD_RW
    sta PORTA
    ora #LCD_EN
    sta PORTA
    bit PORTB
    bne @lcdbusy

    lda #LCD_RW
    sta PORTA
    lda #$ff
    sta DDRB
    pla
.endfunction

.function lcd_inst()
    lcd_wait()
    sta PORTB
    stz PORTA
    lda #LCD_EN
    sta PORTA
    stz PORTA
.endfunction

.function print_char()
    lcd_wait()
    sta PORTB
    lda #LCD_RS
    sta PORTA
    ora #LCD_EN
    sta PORTA
    and #LCD_RS
    sta PORTA
.endfunction

.macro lcd_init(Mode=$38, Input=$0E, Shift=$06)
    lda #Mode ;set 8-bit mode 2line disp 5x8
    lcd_inst()
    lda #Input ;LCD on; Cursor on; blink off
    lcd_inst()
    lda #Shift ; Inc and sht cursor; don't shift display
    lcd_inst()
    lda #CLRDISPLAY ; Clear display
    lcd_inst()
.endm
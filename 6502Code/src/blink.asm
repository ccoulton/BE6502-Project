.target "65C02"
.setting "LaunchCommand", "wsl ../x6502/x6502 {0}"
.memory "fill", $8000, $8000, $ea

.org $8000
lda #$ff
sta $6002
lda #$55
sta $6000
xor #$ff
jmp $8007

.org $fffc
.byte $00
.byte $80

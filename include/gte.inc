# PS1 GTE (COP2) commands; GNU as has no mnemonics for them.
# Encodings taken from the original binary. Add forms here as the assembler reports them.

.macro rtps
    .word 0x4A180001
.endm

.macro rtpt
    .word 0x4A280030
.endm

.macro nclip
    .word 0x4B400006
.endm

.macro nct
    .word 0x4AD80420
.endm

.macro mvmva sf, mx, v, cv, lm
    .word 0x4A400012 | (\sf << 19) | (\mx << 17) | (\v << 15) | (\cv << 13) | (\lm << 10)
.endm

.macro op sf
    .word 0x4B70000C | (\sf << 19)
.endm

.macro sqr sf
    .word 0x4AA00428 | (\sf << 19)
.endm

.macro gpf sf
    .word 0x4B90003D | (\sf << 19)
.endm

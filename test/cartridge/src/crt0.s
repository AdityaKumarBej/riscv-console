.section .init, "ax"
.global _start
_start:
    .cfi_startproc
    .cfi_undefined ra
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, __stack_top
    add s0, sp, zero
    jal ra, init
    nop
    jal zero, main
    .cfi_endproc
    

.section .text, "ax"
.global getTicks, getStatus, genRandom, setGraphicsMode, setTextMode, setColor, calcSmallSpriteControl, calcLargeSpriteControl, calcBackgroundControl
.global setSmallSpriteControl, setLargeSpriteControl, setBackgroundSpriteControl, shiftSmallSpriteControl, shiftLargeSpriteControl, 
.global getSmallSpriteControl, getLargeSpriteControl, getBackgroundSpriteControl
.global printLine, setBackgroundColor
.global InitContext, SwitchContext, getVideoInterruptCount, getCMDInterruptCount
getTicks:
    li a5, 0
    ecall
    ret
getStatus:
    li a5, 1
    ecall
    ret
genRandom:
    li a5, 2
    ecall
    ret
setGraphicsMode:
    li a5, 3
    ecall
    ret
setTextMode:
    li a5, 4
    ecall
    ret
setColor:
    li a5, 5
    ecall
    ret
calcSmallSpriteControl:
    li a5, 6
    ecall
    ret
calcLargeSpriteControl:
    li a5, 7
    ecall
    ret
calcBackgroundControl:
    li a5, 8
    ecall
    ret
setSmallSpriteControl:
    li a5, 9
    ecall
    ret
setLargeSpriteControl:
    li a5, 10
    ecall
    ret
setBackgroundSpriteControl:
    li a5, 11
    ecall
    ret
shiftSmallSpriteControl:
    li a5, 12
    ecall
    ret
shiftLargeSpriteControl:
    li a5, 13
    ecall
    ret
getSmallSpriteControl:
    li a5, 14
    ecall
    ret
getLargeSpriteControl:
    li a5, 15
    ecall
    ret
getBackgroundSpriteControl:
    li a5, 16
    ecall
    ret
printLine:
    li a5, 17
    ecall
    ret
setBackgroundColor:
    li a5, 18
    ecall
    ret
InitContext:
    li a5, 19
    ecall
    ret
SwitchContext:
    li a5, 20
    ecall
    ret
getVideoInterruptCount:
    li a5, 21
    ecall
    ret
getCMDInterruptCount:
    li a5, 22
    ecall
    ret
.end

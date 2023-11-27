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
.global getTicks, getStatus, genRandom, switchToGraphicsMode, switchToTextMode, setColor, generateSmallSpriteConfig, generateLargeSpriteConfig, generateBackgroundConfig
.global drawRectangleWithSmallSprite, drawRectangleWithLargeSprite, drawRectangleWithBackgroundSpriteControl, moveSmallSprite, moveLargeSprite, 
.global getSmallSpriteControl, getLargeSpriteControl, getBackgroundSpriteControl
.global printText, setBackgroundColor
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
switchToGraphicsMode:
    li a5, 3
    ecall
    ret
switchToTextMode:
    li a5, 4
    ecall
    ret
setColor:
    li a5, 5
    ecall
    ret
generateSmallSpriteConfig:
    li a5, 6
    ecall
    ret
generateLargeSpriteConfig:
    li a5, 7
    ecall
    ret
generateBackgroundConfig:
    li a5, 8
    ecall
    ret
drawRectangleWithSmallSprite:
    li a5, 9
    ecall
    ret
drawRectangleWithLargeSprite:
    li a5, 10
    ecall
    ret
drawRectangleWithBackgroundSpriteControl:
    li a5, 11
    ecall
    ret
moveSmallSprite:
    li a5, 12
    ecall
    ret
moveLargeSprite:
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
printText:
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

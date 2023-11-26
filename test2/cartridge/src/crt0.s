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
.global getTicks, getStatus, getVideoToggle, getVidIntCtr, calcLargeSpriteControl, writeVidIntCtrToScreen, sleep, rand, seed_rand, moveLargeSprite, deleteLargeSprite, writeCharToTextPos, writeIntToScreen, setLargeSpriteControl, setSmallSpriteControl, getLargeSpriteControl, getSmallSpriteControl, deleteSmallSprite, getNearbySprites, setBackgroundPalette, setBackgroundControl, setSpritePalette, setPalette, checkCollisionLarge, checkCollisionSmall, checkCollisionLargeSmall
getTicks:
    li a5, 0
    ecall
getStatus:
    li a5, 1
    ecall
getVideoToggle:
    li a5, 2
    ecall
getVidIntCtr:
    li a5, 3
    ecall
calcLargeSpriteControl:
    li a5, 4
    ecall
writeVidIntCtrToScreen:
    li a5, 5
    ecall
sleep:
    li a5, 6
    ecall
rand:
    li a5, 7
    ecall
seed_rand:
    li a5, 8
    ecall
moveLargeSprite:
    li a5, 9
    ecall
deleteLargeSprite:
    li a5, 10
    ecall
writeCharToTextPos:
    li a5, 11
    ecall
writeIntToScreen:
    li a5, 12
    ecall
setLargeSpriteControl:
    li a5, 13
    ecall
setSmallSpriteControl:
    li a5, 14
    ecall
deleteSmallSprite:
    li a5, 15;
    ecall
getLargeSpriteControl:
    li a5, 16
    ecall
getSmallSpriteControl:
    li a5, 17
    ecall
getNearbySprites:
    li a5, 18
    ecall
setBackgroundPalette:
    li a5, 19
    ecall
setBackgroundControl:
    li a5, 20
    ecall
setSpritePalette:
    li a5, 21
    ecall
setPalette:
    li a5, 22
    ecall
checkCollisionLarge:
    li a5, 23
    ecall
checkCollisionSmall:
    li a5, 24
    ecall
checkCollisionLargeSmall:
    li a5, 25
    ecall
.end

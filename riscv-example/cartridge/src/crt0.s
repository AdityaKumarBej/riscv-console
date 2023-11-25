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
.global getTicks, getControllerStatus, getVideoToggle, getVidIntCtr
getTicks:
    li a5, 0
    ecall
    ret
getControllerStatus:
    li a5, 1
    ecall
    ret
getVideoToggle:
    li a5, 2
    ecall
    ret
getVidIntCtr:
    li a5, 3
    ecall
    ret

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
.global getTicks, getStatus

.global InitContext, SwitchContext, getVideoInterruptCount, getCMDInterruptCount
getTicks:
    li a5, 0
    ecall
    ret
getStatus:
    li a5, 1
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

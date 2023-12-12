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

    call    init
    la      ra, main
    mv      a0, gp
    li      a5, 0
    ecall


    .cfi_endproc
    .end

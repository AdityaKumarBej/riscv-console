.section .text, "ax"
.global _interrupt_handler, _system_call
_interrupt_handler:
    csrw    mscratch,ra
    csrr    ra,mcause
    addi    ra,ra,-11
    beqz    ra,_system_call
    csrr    ra,mscratch
    addi	sp,sp,-44
    sw	    gp,40(sp)
    sw	    ra,36(sp)
    sw	    t0,32(sp)
    sw	    t1,28(sp)
    sw	    t2,24(sp)
    sw	    a0,20(sp)
    sw	    a1,16(sp)
    sw	    a2,12(sp)
    sw	    a3,8(sp)
    sw	    a4,4(sp)
    sw	    a5,0(sp)
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    csrr    a0,mcause
    call    c_interrupt_handler
    lw	    gp,40(sp)
    lw	    ra,36(sp)
    lw	    t0,32(sp)
    lw	    t1,28(sp)
    lw	    t2,24(sp)
    lw	    a0,20(sp)
    lw	    a1,16(sp)
    lw	    a2,12(sp)
    lw	    a3,8(sp)
    lw	    a4,4(sp)
    lw	    a5,0(sp)
    addi    sp,sp,44
    mret

_system_call:
    csrr    ra,mscratch
    csrw    mepc,ra
    csrw    mscratch,gp
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    call    c_system_call
    csrr    gp,mscratch
    mret

.section .text, "ax"
.global getTicks, getControllerStatus, getVideoToggle, getVidIntCtr, getCommandStatus, getVideoMemoryDataSystemCall
getTicks:
    li a5, 0
    ecall
getControllerStatus:
    li a5, 1
    ecall
getVideoToggle:
    li a5, 2
    ecall
getVidIntCtr:
    li a5, 3
    ecall
getCommandStatus:
    li a5, 4
    ecall
getVideoMemoryDataSystemCall:
    li a5, 5
    ecall
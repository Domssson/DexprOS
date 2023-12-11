.text

    .global DexprOS_KernelPanicHaltMachine


DexprOS_KernelPanicHaltMachine:
    /* Stay in the halted state even if the machine has received a NMI. */
.halt_loop:
    hlt
    jmp .halt_loop
    ret


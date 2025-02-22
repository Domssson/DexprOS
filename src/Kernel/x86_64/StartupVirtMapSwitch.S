.text
    .global DexprOS_StartupVirtMapSwitchRegionStart
    .global DexprOS_StartupVirtMapSwitchRegionEnd


DexprOS_StartupVirtMapSwitchRegionStart:
    /*
    * Params:
    * rcx - new page map pointer
    * rdx - new stack pointer
    * r8 - the function to be called after a successful Switch
    * r9 - argument to the function that will be called
    */

    /* Switch to the new page map */
    mov %rcx, %cr3

    /* Switch to the new stack */
    mov %rdx, %rsp
    mov %rdx, %rbp

    /* Set the first argument of the function that will be called */
    mov %r9, %rcx
    /* Run the function with new page table! */
    call *%r8

DexprOS_StartupVirtMapSwitchRegionEnd:


.text
    .global DexprOS_LoadGDT

DexprOS_LoadGDT:
    /* Push the size and offset on the stack from the first 2 function arguments */
    pushq %rdx
    pushw %cx

    /* Load the 10-bit GDT address variable from the stack to GDTR */
    lgdt (%rsp)

    /* Pop the previously reserved 10 bits from the stack */
    popw %cx
    popq %rdx

    /* Load the Task State Segment specified by the 5th argument */
    mov 40(%rsp), %ax
    ltr %ax

    /* Move the kernel data segment from the 4th argument to segment control registers */
    mov %r9w, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    
    /* Now flush the CS register */
    /* CS is a segment control register */

    /* Push the kernel code segment on the stack from the 3rd argument*/
    pushq %r8

    /* Load the memory address of flushed section */
    lea .load_gdt_flushed_section(%rip), %rax

    /* Push the section address on the stack */
    pushq %rax

    /* Perform a far return */
    /* Aside of returning it pops a value from the stack to the CS */
    lretq

.load_gdt_flushed_section:
    /* Now simply return to the caller */
    ret


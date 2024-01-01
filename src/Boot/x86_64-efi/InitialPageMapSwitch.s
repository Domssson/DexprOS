.text
    .global DexprOSBoot_Is5LevelPagingActive
    .global DexprOSBoot_SwitchPageMap
    .global DexprOSBoot_SwitchPageMap4LevelTo5Level


DexprOSBoot_Is5LevelPagingActive:
    /* Check bit 12 of CR4 (LA57) */
    mov %cr4, %rax
    test $0x1000, %rax
    jnz .5_level_paging_active
    
    mov $0, %rax
    ret
.5_level_paging_active:
    mov $1, %rax
    ret


DexprOSBoot_SwitchPageMap:
    /* Move the first argument (in RCX) to CR3 control register */
    mov %rcx, %cr3
    ret



DexprOSBoot_SwitchPageMap4LevelTo5Level:

    /* Load a temporary, empty IDT, so the CPU triple faults when a NMI occurs */
    lea tmpIDTRvalue(%rip), %rax
    lidt (%rax)

    /* Fill the temporary 64-bit GDT for later use just after switching back */
    /* from protected mode */

    /* Move the address of tmp64BitGDT to rax */
    lea tmp64BitGDT(%rip), %rax

    /* Kernel code segment*/
    movl $0x0000FFFF, 8(%rax)
    movl $0x00AF9A00, 12(%rax)
    /* Kernel data segment */
    movl $0x0000FFFF, 16(%rax)
    movl $0x00CF9200, 20(%rax)

    /* Fill the 64-bit GDTR value: 32-bit GDT size-1 and its address */
    /* We place a 32-bit address here as it will be accessed in compatibility mode */
    lea tmp64BitGDTRvalue(%rip), %r8
    movw $23, (%r8)
    movl %eax, 2(%r8)


    /* Prepare a far pointer to switch back later */
    lea .switched_back_to_64bit_mode(%rip), %rax
    lea switchBackTo64BitFarPointer(%rip), %r8
    movl %eax, (%r8)
    movw $0x8, 4(%r8)


    /* Fill the temporary 32-bit GDT */
    
    /* Move the address of tmp32BitGDT to rax */
    lea tmp32BitGDT(%rip), %rax

    /* Kernel code segment*/
    movl $0x0000FFFF, 8(%rax)
    movl $0x00CF9A00, 12(%rax)
    /* Kernel data segment */
    movl $0x0000FFFF, 16(%rax)
    movl $0x00CF9200, 20(%rax)

    /* Move the address tmp32BitGDTRvalue to %r8 */
    lea tmp32BitGDTRvalue(%rip), %r8
    /* Fill the GDTR value: 32-bit GDT size-1 and its address */
    movw $23, (%r8)
    movq %rax, 2(%r8)

    /* Now switch the GDT to the 32-bit one */
    lgdt (%r8)


    /* Move the kernel data segment (the 2nd one) to segment control registers */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss


    /* Save values of callee-saved registers */
    pushq %rbx
    pushq %rsi
    pushq %rdi
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15


    lea pageMapRspAndRbpModeSwitchStorage(%rip), %rax
    /* Save the page map pointer (first parameter, in rcx) to the storage space */
    /* The pointer must be in the first 4 GiB of memory, so we can skip the upper bits */
    movl %ecx, (%rax)
    /* Save stack pointer and stack base pointer as their upper bits may be */
    /* lost when switching to protected mode */
    mov %rsp, %r8
    movl %r8d, 4(%rax)
    shr $32, %r8
    movl %r8d, 8(%rax)
    mov %rbp, %r8
    movl %r8d, 12(%rax)
    shr $32, %r8
    movl %r8d, 16(%rax)


    /* Now flush the code segment control register */

    /* Push the kernel code segment (the 1st one) onto the stack */
    pushq $0x8

    /* Load the memory address of 32-bit section and push it onto the stack */
    lea .compatibility_mode_entered(%rip), %rax
    pushq %rax

    /* Load the memory address of 64-bit GDT GDTR value that will be accessed in */
    /* compatibility mode later */
    lea tmp64BitGDTRvalue(%rip), %rbx
    /* Load the address of far jump pointer to return from compatibility mode */
    lea switchBackTo64BitFarPointer(%rip), %rsi
    /* Load the address to the region where previous RSP and RBP are stored */
    lea pageMapRspAndRbpModeSwitchStorage(%rip), %rdi

    /* Perform a far return */
    /* Aside of returning it pops a value from the stack to the CS */
    /* .compatibility_mode_entered will be executed */
    lretq


.switched_back_to_64bit_mode:

    /* Clear caller-saved registers as their upper bits are also undefined */
    /* after switching back from protected mode */
    mov $0, %rax
    mov $0, %rcx
    mov $0, %rdx

    /* Restore stack base pointer */
    mov $0, %r8
    mov $0, %r9
    mov 12(%edi), %r8d
    mov 16(%edi), %r9d
    shl $32, %r9d
    or %r9, %r8
    mov %r8, %rbp

    /* Restore stack pointer */
    mov $0, %r8
    mov $0, %r9
    mov 4(%edi), %r8d
    mov 8(%edi), %r9d
    shl $32, %r9d
    or %r9, %r8
    mov %r8, %rsp

    /* Restore values of callee-saved registers */
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %rdi
    popq %rsi
    popq %rbx

    ret



.bss
    .balign 16
tmp32BitGDTRvalue:
    .skip 10, 0

    .balign 16
tmpIDTRvalue:
    .skip 10


.code32


.bss

    .balign 8
pageMapRspAndRbpModeSwitchStorage:
    .skip 20, 0

    .balign 16
tmp32BitGDT:
    .skip 24, 0

    .balign 16
tmp64BitGDT:
    .skip 24, 0


    .balign 16
tmp64BitGDTRvalue:
    .skip 6, 0

    .balign 16
switchBackTo64BitFarPointer:
    .skip 6, 0


    .balign 4
.text

.compatibility_mode_entered:

    /* Now we're in compatibility long mode */
    /* Now disable paging, it will also automatically disable LMA */
    mov %cr0, %eax
    and $0x7FFFFFFF, %eax
    mov %eax, %cr0

    /* Now clear the LME flag in EFER MSR */
    mov $0xC0000080, %ecx
    rdmsr
    and $0xFFFFFEFF, %eax
    wrmsr

    /* We're now in protected mode */
    

    /* Now we can safely modify paging settings */
    /* Set the LA57 bit in CR4 in order to enable 5-level paging */
    mov %cr4, %eax
    or $0x1000, %eax
    mov %eax, %cr4

    /* Set the 5-level page map from the storage space */
    movl (%edi), %eax
    mov %eax, %cr3


    /* Now set the LME bit in EFER MSR */
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr

    /* Enable paging */
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0


    /* Now we're back in compatibility long mode */

    /* Load the 64-bit temporary GDT */
    lgdt (%ebx)
    /* Perform a far jump with the parameters in the pointer */
    ljmp *(%esi)


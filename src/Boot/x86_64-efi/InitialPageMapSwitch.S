.text
    .global DexprOSBoot_Has5LevelPagingSupport
    .global DexprOSBoot_Is5LevelPagingActive

    .global DexprOSBoot_DisableInterrupts

    .global DexprOSBoot_SwitchPageMap

    .global DexprOSBoot_32BitPageMap5SwitchRegionStart
    .global DexprOSBoot_32BitPageMap5SwitchRegionEnd



DexprOSBoot_Has5LevelPagingSupport:
    /* Save rbx as it's a callee-saved register and it's modified by cpuid */
    push %rbx

    /* Retrieve the maximum number of feature check parameters by calling cpuid with eax=0 */
    mov $0, %rax
    cpuid

    /* Make sure feature set 7 can be retrieved */
    cmp $0x7, %eax
    jge .cpuid_eax7_ecx0_supported

    /* Restore rbx and return 0 on failure */
    pop %rbx
    mov $0, %rax
    ret

.cpuid_eax7_ecx0_supported:
    /* Retrieve feature modes with eax=7, ecx=0 */
    mov $0x7, %eax
    mov $0x0, %ecx
    cpuid

    /* Test bit 16 of ECX */
    test $0x10000, %ecx
    jz .5_level_paging_unsupported

    /* Restore rbx and return 1 on success */
    pop %rbx
    mov $1, %rax
    ret

.5_level_paging_unsupported:
    /* Restore rbx and return 0 on failure */
    pop %rbx
    mov $0, %rax
    ret



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


DexprOSBoot_DisableInterrupts:
    cli
    ret


DexprOSBoot_SwitchPageMap:
    /* Move the first argument (in RCX) to CR3 control register */
    mov %rcx, %cr3
    ret



DexprOSBoot_32BitPageMap5SwitchRegionStart:
    /* The page map address is stored in RCX and the scratch buffer in RDX */


    /* Load a temporary, empty IDT, so the CPU triple faults when a NMI occurs */
    movq $0x0, (%rdx)
    movw $0x0, 8(%rdx)
    lidt (%rdx)


    /* Fill the temporary 64-bit GDT for later use just after switching back */
    /* from protected mode */

    /* The beginning of tmp 64-bit GDT will be stored in RDX+0 */

    /* Null descriptor */
    movq $0x0, (%rdx)

    /* Kernel code segment*/
    movl $0x0000FFFF, 8(%rdx)
    movl $0x00AF9A00, 12(%rdx)

    /* Kernel data segment */
    movl $0x0000FFFF, 16(%rdx)
    movl $0x00CF9200, 20(%rdx)

    /* Fill the 64-bit GDTR value at RDX+32: 32-bit GDT size-1 and its address */
    /* We place a 32-bit GDT address here as it will be accessed in compatibility mode */
    movw $23, 32(%rdx)
    movl %edx, 34(%rdx)


    /* Prepare a far pointer to switch back later at RDX+48 */
    lea .switched_back_to_64bit_mode(%rip), %rax
    movl %eax, 48(%rdx)
    movw $0x8, 52(%rdx)


    /* Fill the temporary 32-bit GDT at RDX+64 */

    /* Null descriptor segment */
    movq $0x0, 64(%rdx)
    /* Kernel code segment*/
    movl $0x0000FFFF, 72(%rdx)
    movl $0x00CF9A00, 76(%rdx)
    /* Kernel data segment */
    movl $0x0000FFFF, 80(%rdx)
    movl $0x00CF9200, 84(%rdx)

    /* Store the address in RAX */
    movq %rdx, %rax
    addq $64, %rax

    /* The temporary GDTR of 32-bit GDT will be stored at RDX+96 */
    /* Fill the GDTR value: 32-bit GDT size-1 and its address */
    movw $23, 96(%rdx)
    movq %rax, 98(%rdx)

    /* Now switch the GDT to the 32-bit one */
    lgdt 96(%rdx)


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


    /* Save the page map pointer (first parameter, in RCX) at RDX+96 */
    /* (we don't need the GDTR now). */
    /* The pointer must be in the first 4 GiB of memory, so we can skip the upper bits */
    movl %ecx, 96(%rdx)
    /* Save stack pointer and stack base pointer as their upper bits may be */
    /* lost when switching to protected mode */
    mov %rsp, %r8
    movl %r8d, 100(%rdx)
    shr $32, %r8
    movl %r8d, 104(%rdx)
    mov %rbp, %r8
    movl %r8d, 108(%rdx)
    shr $32, %r8
    movl %r8d, 112(%rdx)


    /* Now flush the code segment control register */

    /* Push the kernel code segment (the 1st one) onto the stack */
    pushq $0x8

    /* Load the memory address of 32-bit section and push it onto the stack */
    lea .compatibility_mode_entered(%rip), %rax
    pushq %rax

    /* Load the memory address of 64-bit GDT GDTR value that will be accessed in */
    /* compatibility mode later */
    movq %rdx, %rbx
    addq $32, %rbx

    /* Load the address of far jump pointer to return from compatibility mode */
    movq %rdx, %rsi
    addq $48, %rsi

    /* Load the address to the region where the page map address and */
    /* previous RSP and RBP are stored */
    movq %rdx, %rdi
    addq $96, %rdi

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


.code32


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



DexprOSBoot_32BitPageMap5SwitchRegionEnd:


.text
    .global DexprOS_Cpuid


DexprOS_Cpuid:
    /* Move first two parameters to R10 and R11, so they aren't overwritten by cpuid */
    mov %rcx, %r10
    mov %rdx, %r11

    /* Save rbx as it's a callee-saved register and it's modified by cpuid */
    push %rbx

    /* Move contents of first two parameters to EAX and ECX */
    mov $0, %rax
    mov (%r10), %eax
    mov $0, %rcx
    mov (%r11), %ecx

    cpuid

    /* Save cpuid results */
    mov %eax, (%r10)
    mov %ebx, (%r11)
    mov %ecx, (%r8)
    mov %edx, (%r9)

    pop %rbx
    ret


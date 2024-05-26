.text
    .global DexprOS_GetCpuid_EAX1
    .global DexprOS_GetCpuid_EAX7_ECX0


DexprOS_GetCpuid_EAX1:
    /* Move the first two addresses to r10 and r11 as rcx and rdx are modified by cpuid */
    mov %rcx, %r10
    mov %rdx, %r11

    /* Save rbx as it's a callee-saved register and it's modified by cpuid */
    push %rbx

    /* Retrieve the maximum number of feature check parameters by calling cpuid with eax=0 */
    mov $0, %rax
    cpuid

    /* Make sure feature set 1 can be retrieved */
    cmp $0x1, %eax
    jge .cpuid_eax1_supported

    /* Restore rbx and return 0 on failure */
    pop %rbx
    mov $0, %rax
    ret

.cpuid_eax1_supported:
    /* Retrieve feature modes with eax=1 */
    mov $1, %eax
    cpuid

    /* Save output to memory addresses passed as function input */
    /* Notice: we had moved rcx and rdx to r10 and r11 before */
    mov %eax, (%r10)
    mov %ebx, (%r11)
    mov %ecx, (%r8)
    mov %edx, (%r9)

    /* Restore rbx and return 1 on success */
    pop %rbx
    mov $1, %rax
    ret



DexprOS_GetCpuid_EAX7_ECX0:
    /* Move the first two addresses to r10 and r11 as rcx and rdx are modified by cpuid */
    mov %rcx, %r10
    mov %rdx, %r11

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

    /* Save output to memory addresses passed as function input */
    /* Notice: we had moved rcx and rdx to r10 and r11 before */
    mov %eax, (%r10)
    mov %ebx, (%r11)
    mov %ecx, (%r8)
    mov %edx, (%r9)

    /* Restore rbx and return 1 on success */
    pop %rbx
    mov $1, %rax
    ret


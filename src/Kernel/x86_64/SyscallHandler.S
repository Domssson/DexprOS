.text

    .global DexprOS_EnableSyscallExtension

DexprOS_EnableSyscallExtension:
    /* Pack the offsets passed by arguments to a single variable in r8 */
    shl $16, %edx
    mov %edx, %r8d
    mov %cx, %r8w

    /* Read contents of the EFER MSR */
    mov $0xC0000080, %rcx
    rdmsr
    /* Set the sycall extension enable bit and write the value back to EFER */
    or $0x1, %eax
    wrmsr

    /* Write the kernel and user GDT segment offsets to STAR MSR */
    mov $0xC0000081, %rcx
    rdmsr
    mov %r8d, %edx
    wrmsr

    /* LSTAR MSR - stores the 64-bit syscall entry point */
    mov $0xC0000082, %rcx
    lea .SyscallHandler64(%rip), %r8
    rdmsr
    mov %r8d, %eax
    shr $32, %r8
    mov %r8d, %edx
    wrmsr

    /* CSTAR MSR - stores the compatibility syscall entry point */
    mov $0xC0000083, %rcx
    lea .SyscallHandlerCompatibilityIgnore(%rip), %r8
    rdmsr
    mov %r8d, %eax
    shr $32, %r8
    mov %r8d, %edx
    wrmsr

    /* FMASK MSR - stores the rFLAGS bits to clear after sysretq */
    mov $0xC0000084, %rcx
    rdmsr
    /* 0x12BFD5 clears CF, PF, AF, ZF, SF, TF, IF, DF, OF, IOPL, NT, RF, AC and ID */
    mov $0x12BFD5, %eax
    wrmsr

    ret


.SyscallHandler64:
    /* TODO */
    sysretq

.SyscallHandlerCompatibilityIgnore:
    sysretq


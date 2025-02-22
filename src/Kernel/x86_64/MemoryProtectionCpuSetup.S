.text

    .global DexprOS_EnableMemoryProtectionCpuFeatures

DexprOS_EnableMemoryProtectionCpuFeatures:

    /* Enable Write Protect (bit 16 in CR0) */
    mov %cr0, %r8
    or $0x10000, %r8
    mov %r8, %cr0


    /* Save RBX as it's modified by cpuid */
    push %rbx

    /* Check the number of available cpuid feature sets */
    mov $0, %rax
    cpuid

    /* Save the number to R8d */
    mov %eax, %r8d

    /* Save CR4 to R9 in order to modify it */
    mov %cr4, %r9


    cmp $0x80000001, %r8d
    jl .cpuid_eax80000001_unsupported


    /* Test the EAX=80000001h feature set */
    mov $0x80000001, %rax
    cpuid


    /* Set NX (no-execute), bit 20 in cpuid EDX and bit 11 in EFER MSR */
    test $0x100000, %edx
    jz .nx_unsupported
    mov $0xC0000080, %rcx /* EFER MSR */
    rdmsr
    or $0x800, %eax
    wrmsr

    /* Test the EAX=80000001h feature set again as ECX has been overwritten */
    mov $0x80000001, %rax
    cpuid


.nx_unsupported:


    /* Set TCE, bit 17 in cpuid ECX and bit 15 in EFER MSR */
    test $0x20000, %rcx
    jz .tce_unsupported
    mov $0xC0000080, %ecx /* EFER MSR */
    rdmsr
    or $0x8000, %eax
    wrmsr


.tce_unsupported:
.cpuid_eax80000001_unsupported:

    
    cmp $7, %r8d
    jl .cpuid_eax7_unsupported


    /* Test the EAX=7, ECX=0 feature set */
    mov $7, %rax
    mov $0, %rcx
    cpuid

    /* Set SMEP, bit 7 in cpuid EBX and bit 20 in CR4 */
    test $0x80, %ebx
    jz .smep_unsupported
    or $0x100000, %r9

.smep_unsupported:

    /* Set SMAP, bit 20 in cpuid EBX and bit 21 in CR4 */
    test $0x100000, %ebx
    jz .smap_unsupported
    or $0x200000, %r9

.smap_unsupported:

    /* Set PKU, bit 3 in cpuid ECX and bit 22 in CR4 */
    test $0x8, %ecx
    jz .pku_unsupported
    or $0x400000, %r9

.pku_unsupported:
.cpuid_eax7_unsupported:


    cmp $1, %r8d
    jl .cpuid_eax1_unsupported

    /* Test the EAX=1 feature set */
    mov $1, %rax
    cpuid

    /* Set PGE bit (Page global enable), bit 13 in cpuid EDX and bit 7 in CR4 */
    test $0x2000, %edx
    jz .pge_unsupported
    or $0x80, %r9


.pge_unsupported:
.cpuid_eax1_unsupported:

    /* Save the changes and return */
    mov %r9, %cr4
    pop %rbx
    ret

.section .bss
    .align 16
fxsave_buffer_for_mxcsr_modification:
    .skip 512, 0


.section .text
    .extern DexprOS_CheckCpuHasFPU
    .extern DexprOS_CheckCpuHasSSE
    .extern DexprOS_CheckCpuHasFXSAVEAndFXRSTOR
    .extern DexprOS_CheckCpuHasXSAVE
    .extern DexprOS_CheckCpuHasAVX

    .global DexprOS_InitFloatingPointOperations


DexprOS_InitFloatingPointOperations:

    /* Make sure that an FPU is present */
    call DexprOS_CheckCpuHasFPU
    test $0xFF, %al
    jnz .has_onboard_fpu
    ret

.has_onboard_fpu:

    /* Initialize FPU */
    fninit

    /* Set MP, ET and NE bits in CR0 (0b110010) */
    mov %cr0, %rax
    or $0x32, %rax
    /* Clear EM and TS bits in CR0 (0b1100) */
    mov $0xC, %rcx
    not %rcx
    and %rcx, %rax
    /* Store the new value in CR0 */
    mov %rax, %cr0

    /* Check if the CPU has SSE support */
    call DexprOS_CheckCpuHasSSE
    test $0xFF, %al
    jz .no_sse_support

    /* Check if the CPU has FXSAVE and FXRSTOR instructions */
    call DexprOS_CheckCpuHasFXSAVEAndFXRSTOR
    test $0xFF, %al
    jz .no_sse_support

    /* Set CR4 OSFXSR (bit 9) and OSXMMEXCPT (bit 10) bits in order to enable SSE */
    mov %cr4, %rax
    or $0x600, %rax
    mov %rax, %cr4

    /* Set an FXSAVE buffer in order to be able to modify MXCSR */
    fxsave fxsave_buffer_for_mxcsr_modification(%rip)

    /* Setup a new MXCSR register value */
    /* Mask all exception bits (7-12) */
    /* Clear exception flags, rounding control, DAZ and Flush to Zero bits */
    /* Load the new MXCSR register from memory */
    sub $4, %rsp
    movl $0x1F80, (%rsp)
    ldmxcsr (%rsp)
    add $4, %rsp


    /* Check whether XSAVE is supported */
    call DexprOS_CheckCpuHasXSAVE
    test $0xFF, %al
    jz .no_avx_support

    /* Enable OSXSAVE in CR4 */
    mov %cr4, %rax
    or $0x40000, %rax
    mov %rax, %cr4

    /* Check whether AVX is supported */
    call DexprOS_CheckCpuHasAVX
    test $0xFF, %al
    jz .no_avx_support

    /* Enable x87 FPU, SSE and AVX bits in XCR0 */
    mov $0, %rcx
    xgetbv
    or $0x7, %eax
    xsetbv


.no_avx_support:
.no_sse_support:
    ret

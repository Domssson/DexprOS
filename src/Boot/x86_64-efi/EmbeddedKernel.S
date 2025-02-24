.data
    .global DexprOSBoot_KernelExecStart
    .global DexprOSBoot_KernelExecEnd

.balign 4096

DexprOSBoot_KernelExecStart:

    .incbin DEXPROSBOOT_EMBED_KERNEL_PATH

DexprOSBoot_KernelExecEnd:


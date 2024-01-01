#include "DexprOS/Kernel/x86_64/CpuFeatures.h"


bool DexprOS_CheckCpuHasFPU(void)
{
    // FPU bit is ECX 0
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (DexprOS_GetCpuid_EAX1(&eax, &ebx, &ecx, &edx))
        return (edx & 1) == 1;
    return false;
}

bool DexprOS_CheckCpuHasFXSAVEAndFXRSTOR(void)
{
    // fxsr bit is EDX 24
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (DexprOS_GetCpuid_EAX1(&eax, &ebx, &ecx, &edx))
        return (edx & (1 << 24)) == (1 << 24);
    return false;
}

bool DexprOS_CheckCpuHasSSE(void)
{
    // SSE bit is EDX 25
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (DexprOS_GetCpuid_EAX1(&eax, &ebx, &ecx, &edx))
        return (edx & (1 << 25)) == (1 << 25);
    return false;
}

bool DexprOS_CheckCpuHasXSAVE(void)
{
    // XSAVE bit is ECX 26
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (DexprOS_GetCpuid_EAX1(&eax, &ebx, &ecx, &edx))
        return (ecx & (1 << 26)) == (1 << 26);
    return false;
}

bool DexprOS_CheckCpuHasAVX(void)
{
    // AVX bit is ECX 28
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (DexprOS_GetCpuid_EAX1(&eax, &ebx, &ecx, &edx))
        return (ecx & (1 << 28)) == (1 << 28);
    return false;
}


bool DexprOS_CheckCpu5LevelPagingSupport(void)
{
    // (1 << 16) is ecx LA57 bit
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (DexprOS_GetCpuid_EAX7_ECX0(&eax, &ebx, &ecx, &edx))
        return (ecx & (1 << 16)) == (1 << 16);
    return false;
}


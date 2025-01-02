#include "DexprOS/Kernel/x86_64/CpuFeatures.h"


static bool GetCpuidEAX1(uint32_t* pOutEax, uint32_t* pOutEbx, uint32_t* pOutEcx, uint32_t* pOutEdx)
{
    // Check for the presence of feature set 1
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    DexprOS_Cpuid(&eax, &ebx, &ecx, &edx);
    if (eax < 1)
        return false;

    *pOutEax = 1;
    DexprOS_Cpuid(pOutEax, pOutEbx, pOutEcx, pOutEdx);
    return true;
}

static bool GetCpuidEAX80000001h(uint32_t* pOutEax, uint32_t* pOutEbx, uint32_t* pOutEcx, uint32_t* pOutEdx)
{
    // Check for the presence of feature set 0x80000001
    uint32_t eax = 0x80000000, ebx = 0, ecx = 0, edx = 0;
    DexprOS_Cpuid(&eax, &ebx, &ecx, &edx);
    if (eax < 0x80000001)
        return false;

    *pOutEax = 0x80000001;
    DexprOS_Cpuid(pOutEax, pOutEbx, pOutEcx, pOutEdx);
    return true;
}

static bool GetCpuidEAX7_ECX0(uint32_t* pOutEax, uint32_t* pOutEbx, uint32_t* pOutEcx, uint32_t* pOutEdx)
{
    // Check for the presence of feature set 7
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    DexprOS_Cpuid(&eax, &ebx, &ecx, &edx);
    if (eax < 7)
        return false;

    *pOutEax = 7;
    *pOutEcx = 0;
    DexprOS_Cpuid(pOutEax, pOutEbx, pOutEcx, pOutEdx);
    return true;
}


bool DexprOS_CheckCpuHasFPU(void)
{
    // FPU bit is ECX 0
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX1(&eax, &ebx, &ecx, &edx))
        return (edx & 1) == 1;
    return false;
}

bool DexprOS_CheckCpuHasFXSAVEAndFXRSTOR(void)
{
    // fxsr bit is EDX 24
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX1(&eax, &ebx, &ecx, &edx))
        return (edx & (1 << 24)) == (1 << 24);
    return false;
}

bool DexprOS_CheckCpuHasSSE(void)
{
    // SSE bit is EDX 25
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX1(&eax, &ebx, &ecx, &edx))
        return (edx & (1 << 25)) == (1 << 25);
    return false;
}

bool DexprOS_CheckCpuHasXSAVE(void)
{
    // XSAVE bit is ECX 26
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX1(&eax, &ebx, &ecx, &edx))
        return (ecx & (1 << 26)) == (1 << 26);
    return false;
}

bool DexprOS_CheckCpuHasAVX(void)
{
    // AVX bit is ECX 28
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX1(&eax, &ebx, &ecx, &edx))
        return (ecx & (1 << 28)) == (1 << 28);
    return false;
}


bool DexprOS_CheckCpuHasNX(void)
{
    // NX bit is EDX 20
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX80000001h(&eax, &ebx, &ecx, &edx))
        return (edx & (1 << 20)) == (1 << 20);
    return false;
}


bool DexprOS_CheckCpu5LevelPagingSupport(void)
{
    // (1 << 16) is ecx LA57 bit
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    if (GetCpuidEAX7_ECX0(&eax, &ebx, &ecx, &edx))
        return (ecx & (1 << 16)) == (1 << 16);
    return false;
}


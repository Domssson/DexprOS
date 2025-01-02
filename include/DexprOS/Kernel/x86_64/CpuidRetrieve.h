#ifndef DEXPROS_KERNEL_X86_64_CPUIDRETRIEVE_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_CPUIDRETRIEVE_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


extern void DexprOS_Cpuid(uint32_t* pEax, uint32_t* pOutEbx, uint32_t* pEcx, uint32_t* pOutEdx);


#ifdef __cplusplus
}
#endif


#endif // DEXPROS_KERNEL_X86_64_CPUIDRETRIEVE_H_INCLUDED

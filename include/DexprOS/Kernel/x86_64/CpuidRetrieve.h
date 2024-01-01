#ifndef DEXPROS_KERNEL_X86_64_CPUIDRETRIEVE_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_CPUIDRETRIEVE_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <cpuid.h>


extern uint8_t DexprOS_GetCpuid_EAX1(uint32_t* pOutEax, uint32_t* pOutEbx, uint32_t* pOutEcx, uint32_t* pOutEdx);


extern uint8_t DexprOS_GetCpuid_EAX7_ECX0(uint32_t* pOutEax, uint32_t* pOutEbx, uint32_t* pOutEcx, uint32_t* pOutEdx);


#ifdef __cplusplus
}
#endif


#endif // DEXPROS_KERNEL_X86_64_CPUIDRETRIEVE_H_INCLUDED

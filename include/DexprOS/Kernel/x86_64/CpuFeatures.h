#ifndef DEXPROS_KERNEL_X86_64_CPUFEATURES_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_CPUFEATURES_H_INCLUDED

#include "DexprOS/Kernel/x86_64/CpuidRetrieve.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


bool DexprOS_CheckCpuHasFPU(void);

bool DexprOS_CheckCpuHasFXSAVEAndFXRSTOR(void);

bool DexprOS_CheckCpuHasSSE(void);

bool DexprOS_CheckCpuHasXSAVE(void);

bool DexprOS_CheckCpuHasAVX(void);


bool DexprOS_CheckCpu5LevelPagingSupport(void);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEXPROS_KERNEL_X86_64_CPUFEATURES_H_INCLUDED

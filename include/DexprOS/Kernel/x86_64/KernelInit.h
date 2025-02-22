#ifndef DEXPROS_KERNEL_X86_64_KERNELINIT_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_KERNELINIT_H_INCLUDED

#include "KernelStartInfo.h"

#ifdef __cplusplus
extern "C"
{
#endif


void DexprOS_KernelInit(const DexprOS_KernelStartInfo* pStartInfo);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_KERNELINIT_H_INCLUDED

#ifndef DEXPROS_KERNEL_X86_64_SYSCALLHANDLER_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_SYSCALLHANDLER_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


extern void DexprOS_EnableSyscallExtension(uint16_t kernelCodeSegment,
                                           uint16_t userBaseSegment);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_SYSCALLHANDLER_H_INCLUDED

#ifndef DEXPROS_KERNEL_X86_64_STARTUPVIRTMAPSWITCH_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_STARTUPVIRTMAPSWITCH_H_INCLUDED

#include "DexprOS/Kernel/Memory/MemoryDef.h"

#ifdef __cplusplus
extern "C"
{
#endif


extern const char DexprOS_StartupVirtMapSwitchRegionStart[];
extern const char DexprOS_StartupVirtMapSwitchRegionEnd[];

typedef void (*DexprOS_StartupVirtMapSwitchFunc)(DexprOS_PhysicalMemoryAddress table,
                                                 DexprOS_VirtualMemoryAddress stackTopAddr,
                                                 DexprOS_VirtualMemoryAddress callAddr,
                                                 DexprOS_VirtualMemoryAddress callParameter);

#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_STARTUPVIRTMAPSWITCH_H_INCLUDED

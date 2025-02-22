#ifndef DEXPROS_KERNEL_X86_64_STARTUPVIRTMAPCREATOR_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_STARTUPVIRTMAPCREATOR_H_INCLUDED

#include "KernelStartInfo.h"
#include "../Memory/InitialMemMap.h"
#include "../Memory/InitialLinearAllocator.h"

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_BaseStartupInfo
{
    DexprOS_InitialMemMap initialMemMap;

    EFI_SYSTEM_TABLE* pEfiSystemTable;

    DexprOS_EfiMemoryMap efiMemoryMap;

    DexprOS_StartupFramebufferInfo framebuffer;
} DexprOS_BaseStartupInfo;


DexprOS_KernelMapping DexprOS_ChooseStartupVirtMapping(const DexprOS_InitialMemMap* pInitialMap,
                                                       size_t framebufferSize);


// In case of success this function doesn't return
void DexprOS_PerformStartupRemap(const DexprOS_BaseStartupInfo* pBaseStartupInfo,
                                 const DexprOS_KernelMapping* pKernelMapping,
                                 void (*entryFunc)(const DexprOS_KernelStartInfo*));


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_STARTUPVIRTMAPCREATOR_H_INCLUDED

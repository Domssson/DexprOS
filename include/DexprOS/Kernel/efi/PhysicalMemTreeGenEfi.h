#ifndef DEXPROS_KERNEL_EFI_PHYSICALMEMTREEGENEFI_H_INCLUDED
#define DEXPROS_KERNEL_EFI_PHYSICALMEMTREEGENEFI_H_INCLUDED

#include "DexprOS/Kernel/Memory/PhysicalMemTree.h"

#include <efi.h>

#ifdef __cplusplus
extern "C"
{
#endif



size_t DexprOS_GetPhysicalMemTreeSizeFromEfi(const void* pUefiMemoryMap,
                                             UINTN memoryMapSize,
                                             UINTN memoryDescriptorSize,
                                             UINTN memoryDescriptorVersion);



#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_EFI_PHYSICALMEMTREEGENEFI_H_INCLUDED

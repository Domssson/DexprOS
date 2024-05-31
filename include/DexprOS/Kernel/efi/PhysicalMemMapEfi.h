#ifndef DEXPROS_KERNEL_EFI_PHYSICALMEMMAPEFI_H_INCLUDED
#define DEXPROS_KERNEL_EFI_PHYSICALMEMMAPEFI_H_INCLUDED

#include "DexprOS/Kernel/PhysicalMemMap.h"

#include <efi.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


// Returns the required buffer size needed to create a physical memory map
// in DexprOS's format in it.
size_t DexprOS_GetPhysicalMemMapSizeFromEfi(const void* pUefiMemoryMap,
                                            UINTN memoryMapSize,
                                            UINTN memoryDescriptorSize,
                                            UINTN memoryDescriptorVersion);


bool DexprOS_CreatePhysicalMemMapFromEfi(DexprOS_PhysicalMemMap* pResult,
                                         const void* pUefiMemoryMap,
                                         UINTN memoryMapSize,
                                         UINTN memoryDescriptorSize,
                                         UINTN memoryDescriptorVersion,
                                         void* pBuffer,
                                         size_t bufferSize);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_EFI_PHYSICALMEMMAPEFI_H_INCLUDED

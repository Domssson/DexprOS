#ifndef DEXPROS_KERNEL_EFI_PHYSICALMEMMAPEFI_H_INCLUDED
#define DEXPROS_KERNEL_EFI_PHYSICALMEMMAPEFI_H_INCLUDED

#include "DexprOS/Kernel/Memory/PhysicalMemMap.h"

#include <efi.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


// Sets the required size and alignment of the buffer needed to create the new
// memory map
void DexprOS_GetPhysicalMemMapSizeFromEfi(const void* pUefiMemoryMap,
                                          UINTN memoryMapSize,
                                          UINTN memoryDescriptorSize,
                                          UINTN memoryDescriptorVersion,
                                          size_t* pOutBufferSize,
                                          size_t* pOutBufferAlignment);


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

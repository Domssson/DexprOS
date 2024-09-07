#ifndef DEXPROS_KERNEL_EFI_EFIMEMMAPITERATION_H_INCLUDED
#define DEXPROS_KERNEL_EFI_EFIMEMMAPITERATION_H_INCLUDED

#include "DexprOS/Kernel/PhysicalMemMap.h"

#include <efi.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef void (*DexprOS_EfiMemMapIterateFunc)(DexprOS_PhysicalMemoryAddress physicalAddress,
                                             DexprOS_PhysicalMemorySize rangeSize,
                                             DexprOS_PhysicalMemoryType memType,
                                             DexprOS_PhysicalMemoryFlags memFlags,
                                             void* pUserData);


void DexprOS_IterateEfiMemMap(const void* pUefiMemoryMap,
                              UINTN memoryMapSize,
                              UINTN memoryDescriptorSize,
                              UINTN memoryDescriptorVersion,
                              const DexprOS_EfiMemMapIterateFunc pFunc,
                              void* pUserData);



#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_EFI_EFIMEMMAPITERATION_H_INCLUDED

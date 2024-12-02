#ifndef DEXPROS_KERNEL_EFI_INITIALMEMMAPGENEFI_H_INCLUDED
#define DEXPROS_KERNEL_EFI_INITIALMEMMAPGENEFI_H_INCLUDED

#include "DexprOS/Kernel/Memory/InitialMemMap.h"

#include <efi.h>

#include <stddef.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C"
{
#endif


bool DexprOS_CreateInitialMemMapFromEfi(const void* pUefiMemoryMap,
                                        UINTN memoryMapSize,
                                        UINTN memoryDescriptorSize,
                                        UINTN memoryDescriptorVersion,
                                        DexprOS_InitialMemMap* pOutMemMap);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_EFI_INITIALMEMMAPGENEFI_H_INCLUDED

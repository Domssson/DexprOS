#ifndef DEXPROS_KERNEL_EFI_PHYSICALMEMSTRUCTSGENEFI_H_INCLUDED
#define DEXPROS_KERNEL_EFI_PHYSICALMEMSTRUCTSGENEFI_H_INCLUDED

#include "DexprOS/Kernel/efi/PhysicalMemTreeGenEfi.h"
#include "DexprOS/Kernel/efi/PhysicalMemMapEfi.h"
#include "DexprOS/Kernel/Memory/PhysicalMemTree.h"
#include "DexprOS/Kernel/Memory/PhysicalMemMap.h"

#include <efi.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*
* This struct holds size and alignment requirements for the buffer
* that will store the OS's physical memory management structures
* and caches offsets and other info useful for creation of the structures
*/
typedef struct DexprOS_PhysMemStructsEfiSizeData
{
    size_t bufferSize;
    size_t bufferAlignment;

    size_t treeOffset;
    DexprOS_PhysMemTreeEfiSizeData treeSizeData;

    size_t memMapOffset;
    size_t memMapSize;
} DexprOS_PhysMemStructsEfiSizeData;



DexprOS_PhysMemStructsEfiSizeData
DexprOS_GetPhysicalMemStructsSizeDataFromEfi(const void* pUefiMemoryMap,
                                             UINTN memoryMapSize,
                                             UINTN memoryDescriptorSize,
                                             UINTN memoryDescriptorVersion);


bool DexprOS_CreatePhysicalMemStructsFromEfi(DexprOS_PhysicalMemTree* pOutTree,
                                             DexprOS_PhysicalMemMap* pOutMemMap,
                                             const void* pUefiMemoryMap,
                                             UINTN memoryMapSize,
                                             UINTN memoryDescriptorSize,
                                             UINTN memoryDescriptorVersion,
                                             void* pBuffer,
                                             const DexprOS_PhysMemStructsEfiSizeData* pSizeData);



#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_EFI_PHYSICALMEMSTRUCTSGENEFI_H_INCLUDED

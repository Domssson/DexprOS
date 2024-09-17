#ifndef DEXPROS_KERNEL_EFI_PHYSICALMEMTREEGENEFI_H_INCLUDED
#define DEXPROS_KERNEL_EFI_PHYSICALMEMTREEGENEFI_H_INCLUDED

#include "DexprOS/Kernel/Memory/PhysicalMemTree.h"

#include <efi.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


// A struct storing buffer allocation requirements and caches other data needed
// to create the physical memory tree
typedef struct DexprOS_PhysMemTreeEfiSizeData
{
    size_t bufferAlignment;
    size_t bufferSize;

    size_t treeEntriesOffset;
    size_t numTreeEntries;

    size_t treeLevelStructsOffset;
    size_t numTreeLevelStructs;

    size_t blocksOffset;
    size_t totalBlocksSize;
} DexprOS_PhysMemTreeEfiSizeData;


DexprOS_PhysMemTreeEfiSizeData
DexprOS_GetPhysicalMemTreeSizeFromEfi(const void* pUefiMemoryMap,
                                      UINTN memoryMapSize,
                                      UINTN memoryDescriptorSize,
                                      UINTN memoryDescriptorVersion);


bool DexprOS_CreatePhysicalMemTreeFromEfi(DexprOS_PhysicalMemTree* pResult,
                                          const void* pUefiMemoryMap,
                                          UINTN memoryMapSize,
                                          UINTN memoryDescriptorSize,
                                          UINTN memoryDescriptorVersion,
                                          void* pBuffer,
                                          const DexprOS_PhysMemTreeEfiSizeData* pSizeData);


void DexprOS_MarkPhysicalMemTreeRegionsFromEfi(const DexprOS_PhysicalMemTree* pTree,
                                               const void* pUefiMemoryMap,
                                               UINTN memoryMapSize,
                                               UINTN memoryDescriptorSize,
                                               UINTN memoryDescriptorVersion,
                                               size_t numOtherRegionsInUse,
                                               const DexprOS_PhysicalMemoryAddress* pOtherRegionsInUse,
                                               const DexprOS_PhysicalMemorySize* pOtherRegionsSizes);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_EFI_PHYSICALMEMTREEGENEFI_H_INCLUDED

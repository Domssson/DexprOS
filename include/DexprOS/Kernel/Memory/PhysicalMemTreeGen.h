#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREEGEN_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREEGEN_H_INCLUDED

#include "PhysicalMemTree.h"

#include "InitialMemMap.h"

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


// A struct storing buffer allocation requirements and caches other data needed
// to create the physical memory tree
typedef struct DexprOS_PhysMemTreeSizeData
{
    size_t bufferAlignment;
    size_t bufferSize;

    size_t treeEntriesOffset;
    size_t numTreeEntries;

    size_t treeLevelStructsOffset;
    size_t numTreeLevelStructs;

    size_t blocksOffset;
    size_t totalBlocksSize;
} DexprOS_PhysMemTreeSizeData;


DexprOS_PhysMemTreeSizeData
DexprOS_GetPhysicalMemTreeSize(const DexprOS_InitialMemMap* pInitialMap);


bool DexprOS_CreatePhysicalMemTree(DexprOS_PhysicalMemTree* pResult,
                                   const DexprOS_InitialMemMap* pInitialMap,
                                   void* pBuffer,
                                   const DexprOS_PhysMemTreeSizeData* pSizeData);


void DexprOS_MarkPhysicalMemTreeRegions(const DexprOS_PhysicalMemTree* pTree,
                                        const DexprOS_InitialMemMap* pInitialMap,
                                        size_t numOtherRegionsInUse,
                                        const DexprOS_PhysicalMemoryAddress* pOtherRegionsInUse,
                                        const DexprOS_PhysicalMemorySize* pOtherRegionsSizes);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREEGEN_H_INCLUDED

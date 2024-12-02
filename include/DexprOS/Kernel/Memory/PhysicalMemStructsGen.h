#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMSTRUCTSGEN_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMSTRUCTSGEN_H_INCLUDED

#include "PhysicalMemMap.h"
#include "PhysicalMemTree.h"
#include "PhysicalMemMapGen.h"
#include "PhysicalMemTreeGen.h"
#include "InitialMemMap.h"

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
typedef struct DexprOS_PhysMemStructsSizeData
{
    size_t bufferSize;
    size_t bufferAlignment;

    size_t treeOffset;
    DexprOS_PhysMemTreeSizeData treeSizeData;

    size_t memMapOffset;
    size_t memMapSize;
} DexprOS_PhysMemStructsSizeData;



DexprOS_PhysMemStructsSizeData
DexprOS_GetPhysicalMemStructsSizeData(const DexprOS_InitialMemMap* pInitialMap);


bool DexprOS_CreatePhysicalMemStructs(DexprOS_PhysicalMemTree* pOutTree,
                                      DexprOS_PhysicalMemMap* pOutMemMap,
                                      const DexprOS_InitialMemMap* pInitialMap,
                                      void* pBuffer,
                                      const DexprOS_PhysMemStructsSizeData* pSizeData);



#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_MEMORY_PHYSICALMEMSTRUCTSGEN_H_INCLUDED

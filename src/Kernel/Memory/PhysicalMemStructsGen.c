#include "DexprOS/Kernel/Memory/PhysicalMemStructsGen.h"

#include "DexprOS/Kernel/Memory/MemoryDef.h"


DexprOS_PhysMemStructsSizeData
DexprOS_GetPhysicalMemStructsSizeData(const DexprOS_InitialMemMap* pInitialMap)
{
    DexprOS_PhysMemStructsSizeData sizeData;


    sizeData.treeSizeData = DexprOS_GetPhysicalMemTreeSize(pInitialMap);

    size_t treeSize = sizeData.treeSizeData.bufferSize;
    size_t treeAlignment = sizeData.treeSizeData.bufferAlignment;
    sizeData.treeOffset = 0;


    size_t mapSize = 0;
    size_t mapAlignment = 0;
    DexprOS_GetPhysicalMemMapSize(pInitialMap,
                                  &mapSize,
                                  &mapAlignment);

    sizeData.memMapOffset = DEXPROS_ALIGN(treeSize, mapAlignment);
    sizeData.memMapSize = mapSize;


    sizeData.bufferSize = sizeData.memMapOffset + mapSize;
    sizeData.bufferAlignment = (mapAlignment > treeAlignment ? mapAlignment : treeAlignment);
    
    return sizeData;
}


bool DexprOS_CreatePhysicalMemStructs(DexprOS_PhysicalMemTree* pOutTree,
                                      DexprOS_PhysicalMemMap* pOutMemMap,
                                      const DexprOS_InitialMemMap* pInitialMap,
                                      void* pBuffer,
                                      const DexprOS_PhysMemStructsSizeData* pSizeData)
{
    char* pTreeBuffer = (char*)pBuffer + pSizeData->treeOffset;

    if (!DexprOS_CreatePhysicalMemTree(pOutTree,
                                       pInitialMap,
                                       (void*)pTreeBuffer,
                                       &pSizeData->treeSizeData))
        return false;

    DexprOS_PhysicalMemoryAddress bufferAddress = (DexprOS_PhysicalMemoryAddress)pBuffer;
    DexprOS_PhysicalMemorySize bufferSize = pSizeData->bufferSize;
    DexprOS_MarkPhysicalMemTreeRegions(pOutTree,
                                       pInitialMap,
                                       1,
                                       &bufferAddress,
                                       &bufferSize);
    


    char* pMapBuffer = (char*)pBuffer + pSizeData->memMapOffset;

    if (!DexprOS_CreatePhysicalMemMap(pOutMemMap,
                                      pInitialMap,
                                      (void*)pMapBuffer,
                                      pSizeData->memMapSize))
        return false;

    return true;
}


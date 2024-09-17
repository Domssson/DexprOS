#include "DexprOS/Kernel/efi/PhysicalMemStructsGenEfi.h"

#include "DexprOS/Kernel/Memory/MemoryDef.h"


DexprOS_PhysMemStructsEfiSizeData
DexprOS_GetPhysicalMemStructsSizeDataFromEfi(const void* pUefiMemoryMap,
                                             UINTN memoryMapSize,
                                             UINTN memoryDescriptorSize,
                                             UINTN memoryDescriptorVersion)
{
    DexprOS_PhysMemStructsEfiSizeData sizeData;


    sizeData.treeSizeData = DexprOS_GetPhysicalMemTreeSizeFromEfi(pUefiMemoryMap,
                                                                  memoryMapSize,
                                                                  memoryDescriptorSize,
                                                                  memoryDescriptorVersion);

    size_t treeSize = sizeData.treeSizeData.bufferSize;
    size_t treeAlignment = sizeData.treeSizeData.bufferAlignment;
    sizeData.treeOffset = 0;


    size_t mapSize = 0;
    size_t mapAlignment = 0;
    DexprOS_GetPhysicalMemMapSizeFromEfi(pUefiMemoryMap,
                                         memoryMapSize,
                                         memoryDescriptorSize,
                                         memoryDescriptorVersion,
                                         &mapSize,
                                         &mapAlignment);

    sizeData.memMapOffset = DEXPROS_ALIGN(treeSize, mapAlignment);
    sizeData.memMapSize = mapSize;


    sizeData.bufferSize = sizeData.memMapOffset + mapSize;
    sizeData.bufferAlignment = (mapAlignment > treeAlignment ? mapAlignment : treeAlignment);
    
    return sizeData;
}


bool DexprOS_CreatePhysicalMemStructsFromEfi(DexprOS_PhysicalMemTree* pOutTree,
                                             DexprOS_PhysicalMemMap* pOutMemMap,
                                             const void* pUefiMemoryMap,
                                             UINTN memoryMapSize,
                                             UINTN memoryDescriptorSize,
                                             UINTN memoryDescriptorVersion,
                                             void* pBuffer,
                                             const DexprOS_PhysMemStructsEfiSizeData* pSizeData)
{
    char* pTreeBuffer = (char*)pBuffer + pSizeData->treeOffset;

    if (!DexprOS_CreatePhysicalMemTreeFromEfi(pOutTree,
                                              pUefiMemoryMap,
                                              memoryMapSize,
                                              memoryDescriptorSize,
                                              memoryDescriptorVersion,
                                              (void*)pTreeBuffer,
                                              &pSizeData->treeSizeData))
        return false;

    DexprOS_PhysicalMemoryAddress bufferAddress = (DexprOS_PhysicalMemoryAddress)pBuffer;
    DexprOS_PhysicalMemorySize bufferSize = pSizeData->bufferSize;
    DexprOS_MarkPhysicalMemTreeRegionsFromEfi(pOutTree,
                                              pUefiMemoryMap,
                                              memoryMapSize,
                                              memoryDescriptorSize,
                                              memoryDescriptorVersion,
                                              1,
                                              &bufferAddress,
                                              &bufferSize);
    


    char* pMapBuffer = (char*)pBuffer + pSizeData->memMapOffset;

    if (!DexprOS_CreatePhysicalMemMapFromEfi(pOutMemMap,
                                             pUefiMemoryMap,
                                             memoryMapSize,
                                             memoryDescriptorSize,
                                             memoryDescriptorVersion,
                                             (void*)pMapBuffer,
                                             pSizeData->memMapSize))
        return false;

    return true;
}


#include "DexprOS/Kernel/Memory/PhysicalMemTreeGen.h"

#include "DexprOS/Kernel/Memory/Paging.h"
#include "DexprOS/Kernel/Memory/MemoryDef.h"
#include "DexprOS/Kernel/kstdlib/string.h"

#include <stdbool.h>


static bool ShouldCreateTreeForType(DexprOS_PhysicalMemoryType memType)
{
    switch (memType)
    {
    // Do not include boot services memory for allocation. There is a bug in
    // some UEFI implementations that causes the runtime services to access
    // boot services memory (https://wiki.osdev.org/Broken_UEFI_implementations).
    case DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE_PERSISTENT:
        return true;
    
    case DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_Max:
        return false;
    }
    return false;
}


static void CalcTreeEntryElements(DexprOS_PhysicalMemoryAddress physicalAddress,
                                  DexprOS_PhysicalMemorySize rangeSize,
                                  DexprOS_PhysicalMemoryType memType,
                                  DexprOS_PhysicalMemoryFlags memFlags,
                                  void* pUserData)
{
    if (!ShouldCreateTreeForType(memType))
        return;

    DexprOS_PhysMemTreeSizeData* pSizeData = (DexprOS_PhysMemTreeSizeData*)pUserData;
    pSizeData->numTreeEntries += 1;

    
    size_t blockSize = DEXPROS_PHYSICAL_PAGE_SIZE;
    DexprOS_PhysicalMemoryAddress minMappedAddress = physicalAddress;
    DexprOS_PhysicalMemoryAddress maxMappedAddress = physicalAddress + rangeSize;

    for (;;)
    {
        pSizeData->numTreeLevelStructs += 1;

        minMappedAddress = (minMappedAddress / blockSize) * blockSize;
        maxMappedAddress = ((maxMappedAddress + blockSize - 1) / blockSize) * blockSize;

        size_t numBlocks = (maxMappedAddress - minMappedAddress) / blockSize;
        if (numBlocks > sizeof(uint8_t*))
            pSizeData->totalBlocksSize += numBlocks;

        if (numBlocks <= 1)
            return;

        blockSize *= 2;
    }


    (void)physicalAddress;
    (void)memFlags;
}


DexprOS_PhysMemTreeSizeData
DexprOS_GetPhysicalMemTreeSize(const DexprOS_InitialMemMap* pInitialMap)
{
    DexprOS_PhysMemTreeSizeData sizeData;
    sizeData.bufferAlignment = DEXPROS_FUNDAMENTAL_ALIGNMENT;
    sizeData.bufferSize = 0;
    sizeData.treeEntriesOffset = 0;
    sizeData.numTreeEntries = 0;
    sizeData.treeLevelStructsOffset = 0;
    sizeData.numTreeLevelStructs = 0;
    sizeData.blocksOffset = 0;
    sizeData.totalBlocksSize = 0;

    DexprOS_IterateInitialMemMapCombinedPhysRanges(pInitialMap,
                                                   CalcTreeEntryElements,
                                                   (void*)&sizeData);

    size_t bufferSize = 0;

    bufferSize += sizeof(DexprOS_PhysicalMemTreeEntry) * sizeData.numTreeEntries;
    // Align the size so the next array of structs can be safely stored
    bufferSize = DEXPROS_ALIGN_FUNDAMENTAL(bufferSize);

    sizeData.treeLevelStructsOffset = bufferSize;

    bufferSize += sizeof(DexprOS_PhysicalMemTreeLevel) * sizeData.numTreeLevelStructs;
    bufferSize = DEXPROS_ALIGN_FUNDAMENTAL(bufferSize);

    sizeData.blocksOffset = bufferSize;

    bufferSize += sizeData.totalBlocksSize;

    sizeData.bufferSize = bufferSize;

    return sizeData;
}


typedef struct DexprOS_PhysMemTreeCreateData
{
    void* pBuffer;
    const DexprOS_PhysMemTreeSizeData* pSizeData;

    DexprOS_PhysicalMemTreeEntry* pEntries;
    DexprOS_PhysicalMemTreeLevel* pLevels;
    uint8_t* pBlocks;

    size_t treeEntryIndex;
    size_t treeLevelStructIndex;
    size_t blockIndex;


    bool success;
} PhysMemTreeCreateData;


static bool CreateTreeLevelBlocks(PhysMemTreeCreateData* pCreateData,
                                  DexprOS_PhysicalMemTreeLevel* pLevel)
{
    if (pLevel->numBlocks > sizeof(pLevel->blocks.inPlaceBlocks))
    {
        pLevel->blocks.pBlocks = &pCreateData->pBlocks[pCreateData->blockIndex];
        pCreateData->blockIndex += pLevel->numBlocks;

        if (pCreateData->blockIndex > pCreateData->pSizeData->totalBlocksSize)
            return false;
        

        memset(pLevel->blocks.pBlocks, 0, pLevel->numBlocks);
    }
    else
    {
        memset(pLevel->blocks.inPlaceBlocks, 0, sizeof(pLevel->blocks.inPlaceBlocks));
    }

    return true;
}


static bool CreateTreeLevels(PhysMemTreeCreateData* pCreateData,
                             DexprOS_PhysicalMemTreeEntry* pEntry)
{
    size_t blockSize = DEXPROS_PHYSICAL_PAGE_SIZE;
    DexprOS_PhysicalMemoryAddress minMappedAddress = pEntry->startAddress;
    DexprOS_PhysicalMemoryAddress maxMappedAddress = pEntry->endAddress;

    // Calculate number of levels

    size_t numLevels = 0;

    for (;;)
    {
        numLevels += 1;

        minMappedAddress = (minMappedAddress / blockSize) * blockSize;
        maxMappedAddress = ((maxMappedAddress + blockSize - 1) / blockSize) * blockSize;

        size_t numBlocks = (maxMappedAddress - minMappedAddress) / blockSize;

        if (numBlocks <= 1)
            break;

        blockSize *= 2;
    }


    pEntry->numTreeLevels = numLevels;
    pCreateData->treeLevelStructIndex += numLevels;

    if (pCreateData->treeLevelStructIndex > pCreateData->pSizeData->numTreeLevelStructs)
        return false;

    return true;
}


static bool FillTreeLevelStructs(PhysMemTreeCreateData* pCreateData,
                                 DexprOS_PhysicalMemTreeEntry* pEntry)
{
    // Fill level structs
    size_t blockSize = DEXPROS_PHYSICAL_PAGE_SIZE;
    DexprOS_PhysicalMemoryAddress minMappedAddress = pEntry->startAddress;
    DexprOS_PhysicalMemoryAddress maxMappedAddress = pEntry->endAddress;

    for (size_t i = 0; i < pEntry->numTreeLevels; ++i)
    {
        minMappedAddress = (minMappedAddress / blockSize) * blockSize;
        maxMappedAddress = ((maxMappedAddress + blockSize - 1) / blockSize) * blockSize;

        size_t iLevel = pEntry->numTreeLevels - i - 1;

        DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[iLevel];
        pLevel->startMappedAddress = minMappedAddress;
        pLevel->blockSize = blockSize;
        pLevel->numBlocks = (maxMappedAddress - minMappedAddress) / blockSize;

        blockSize *= 2;
    }

    
    // Create blocks inside levels

    for (size_t i = 0; i < pEntry->numTreeLevels; ++i)
    {
        DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[i];

        if (!CreateTreeLevelBlocks(pCreateData, pLevel))
            return false;
    }

    return true;
}


static void CreateTreeForMemoryRange(DexprOS_PhysicalMemoryAddress physicalAddress,
                                     DexprOS_PhysicalMemorySize rangeSize,
                                     DexprOS_PhysicalMemoryType memType,
                                     DexprOS_PhysicalMemoryFlags memFlags,
                                     void* pUserData)
{
    if (!ShouldCreateTreeForType(memType))
        return;


    PhysMemTreeCreateData* pCreateData = (PhysMemTreeCreateData*)pUserData;

    // Create new entry and fill the pNextEntry field in the previous entry
    DexprOS_PhysicalMemTreeEntry* pEntry = &pCreateData->pEntries[pCreateData->treeEntryIndex];
    if (pCreateData->treeEntryIndex > 0)
    {
        DexprOS_PhysicalMemTreeEntry* pPrevEntry = pEntry - 1;
        pPrevEntry->pNextEntry = pEntry;
    }
    pCreateData->treeEntryIndex += 1;

    if (pCreateData->treeEntryIndex > pCreateData->pSizeData->numTreeEntries)
    {
        pCreateData->success = false;
        return;
    }


    pEntry->type = memType;
    pEntry->flags = memFlags;
    pEntry->startAddress = physicalAddress;
    pEntry->endAddress = physicalAddress + rangeSize;
    pEntry->numPages = rangeSize / DEXPROS_PHYSICAL_PAGE_SIZE;
    pEntry->numTreeLevels = 0;
    pEntry->pTreeLevels = &pCreateData->pLevels[pCreateData->treeLevelStructIndex];
    pEntry->pNextEntry = NULL;


    if (!CreateTreeLevels(pCreateData, pEntry))
        pCreateData->success = false;

    if (!FillTreeLevelStructs(pCreateData, pEntry))
        pCreateData->success = false;
}


bool DexprOS_CreatePhysicalMemTree(DexprOS_PhysicalMemTree* pResult,
                                   const DexprOS_InitialMemMap* pInitialMap,
                                   void* pBuffer,
                                   const DexprOS_PhysMemTreeSizeData* pSizeData)
{
    PhysMemTreeCreateData createData;
    createData.pBuffer = pBuffer;
    createData.pSizeData = pSizeData;

    createData.pEntries = (DexprOS_PhysicalMemTreeEntry*)((char*)pBuffer + pSizeData->treeEntriesOffset);
    createData.pLevels = (DexprOS_PhysicalMemTreeLevel*)((char*)pBuffer + pSizeData->treeLevelStructsOffset);
    createData.pBlocks = (uint8_t*)((char*)pBuffer + pSizeData->blocksOffset);

    createData.treeEntryIndex = 0;
    createData.treeLevelStructIndex = 0;
    createData.blockIndex = 0;

    createData.success = true;

    DexprOS_IterateInitialMemMapCombinedPhysRanges(pInitialMap,
                                                   CreateTreeForMemoryRange,
                                                   (void*)&createData);

    pResult->pFirstEntry = createData.pEntries;


    return createData.success;
}


static void SetAllTreePagesAvailable(const DexprOS_PhysicalMemTree* pTree)
{
    DexprOS_PhysicalMemTreeEntry* pEntry = pTree->pFirstEntry;

    while (pEntry != NULL)
    {
        DexprOS_PhysicalMemTreeLevel* pLastLevel = &pEntry->pTreeLevels[pEntry->numTreeLevels - 1];

        uint8_t* pBlocks;

        if (pLastLevel->numBlocks > sizeof(pLastLevel->blocks.inPlaceBlocks))
            pBlocks = pLastLevel->blocks.pBlocks;
        else
            pBlocks = pLastLevel->blocks.inPlaceBlocks;

        memset(pBlocks, DEXPROS_PHYS_MEM_PAGE_AVAILABLE_BIT, pLastLevel->numBlocks);

        pEntry = pEntry->pNextEntry;
    }
}


static void MarkUsedTreePages(const DexprOS_PhysicalMemTree* pTree,
                              DexprOS_PhysicalMemoryAddress regionStartAddress,
                              DexprOS_PhysicalMemorySize regionSize)
{
    DexprOS_PhysicalMemoryAddress startAddress = (regionStartAddress / DEXPROS_PHYSICAL_PAGE_SIZE) *
                                                 DEXPROS_PHYSICAL_PAGE_SIZE;
    DexprOS_PhysicalMemoryAddress endAddress = DEXPROS_ALIGN(regionStartAddress + regionSize,
                                                             DEXPROS_PHYSICAL_PAGE_SIZE);

    DexprOS_PhysicalMemTreeEntry* pEntry = pTree->pFirstEntry;

    while (pEntry != NULL)
    {
        if (startAddress < pEntry->endAddress &&
            endAddress > pEntry->startAddress)
        {
            DexprOS_PhysicalMemTreeLevel* pLastLevel = &pEntry->pTreeLevels[pEntry->numTreeLevels - 1];
            DexprOS_PhysicalMemoryAddress minMapAddress = pLastLevel->startMappedAddress;
            DexprOS_PhysicalMemoryAddress maxMapAddress = pLastLevel->startMappedAddress + pLastLevel->numBlocks * pLastLevel->blockSize;

            minMapAddress = (startAddress > minMapAddress ? startAddress : minMapAddress);
            maxMapAddress = (endAddress < maxMapAddress ? endAddress : maxMapAddress);

            size_t iFirstBlock = (minMapAddress - pLastLevel->startMappedAddress) / pLastLevel->blockSize;
            size_t numMarkBlocks = (maxMapAddress - minMapAddress) / pLastLevel->blockSize;

            uint8_t* pBlocks;

            if (pLastLevel->numBlocks > sizeof(pLastLevel->blocks.inPlaceBlocks))
                pBlocks = pLastLevel->blocks.pBlocks;
            else
                pBlocks = pLastLevel->blocks.inPlaceBlocks;

            memset(pBlocks + iFirstBlock, DEXPROS_PHYS_MEM_PAGE_UNAVAILABLE, numMarkBlocks);
        }

        pEntry = pEntry->pNextEntry;
    }
}


static void FillTreeLevelAccelBlocks(DexprOS_PhysicalMemTreeEntry* pEntry, size_t iLevel)
{
    DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[iLevel];
    DexprOS_PhysicalMemTreeLevel* pNextLevel = &pEntry->pTreeLevels[iLevel + 1];

    size_t numBlocks = pLevel->numBlocks;
    size_t numNextBlocks = pNextLevel->numBlocks;
    
    uint8_t* pBlocks = (numBlocks > sizeof(pLevel->blocks.inPlaceBlocks) ?
                        pLevel->blocks.pBlocks : pLevel->blocks.inPlaceBlocks);

    uint8_t* pNextBlocks = (numNextBlocks > sizeof(pNextLevel->blocks.inPlaceBlocks) ?
                            pNextLevel->blocks.pBlocks : pNextLevel->blocks.inPlaceBlocks);
    
    
    size_t minNextOffset = (pNextLevel->startMappedAddress - pLevel->startMappedAddress) /
                            pNextLevel->blockSize;
    size_t maxNextOffset = minNextOffset + numNextBlocks;


    for (size_t i = 0; i < numBlocks; ++i)
    {
        size_t iNext0 = i * 2;
        size_t iNext1 = i * 2 + 1;

        uint8_t state0 = 0, state1 = 0;

        if (iNext0 >= minNextOffset && iNext0 < maxNextOffset)
            state0 = pNextBlocks[iNext0 - minNextOffset];
        if (iNext1 >= minNextOffset && iNext1 < maxNextOffset)
            state1 = pNextBlocks[iNext1 - minNextOffset];


        uint8_t result = ((state0 >> 1) + (state1 >> 1));
        if ((state0 & DEXPROS_PHYS_MEM_BLOCK_1_128_OR_LESS_AVAILABLE_BIT) != 0 ||
            (state1 & DEXPROS_PHYS_MEM_BLOCK_1_128_OR_LESS_AVAILABLE_BIT) != 0)
            result |= DEXPROS_PHYS_MEM_BLOCK_1_128_OR_LESS_AVAILABLE_BIT;
        
        pBlocks[i] = result;
    }
}


static void FillTreeAccelBlocks(const DexprOS_PhysicalMemTree* pTree)
{
    DexprOS_PhysicalMemTreeEntry* pEntry = pTree->pFirstEntry;

    while (pEntry != NULL)
    {
        for (size_t i = 1; i < pEntry->numTreeLevels; ++i)
        {
            size_t iLevel = pEntry->numTreeLevels - i - 1;

            FillTreeLevelAccelBlocks(pEntry, iLevel);
        }

        pEntry = pEntry->pNextEntry;
    }
}


void DexprOS_MarkPhysicalMemTreeRegions(const DexprOS_PhysicalMemTree* pTree,
                                        const DexprOS_InitialMemMap* pInitialMap,
                                        size_t numOtherRegionsInUse,
                                        const DexprOS_PhysicalMemoryAddress* pOtherRegionsInUse,
                                        const DexprOS_PhysicalMemorySize* pOtherRegionsSizes)
{
    SetAllTreePagesAvailable(pTree);

    // Mark kernel, boot and initial map regions on the map because they're included
    // in the OS's usable regions managed by the tree and are currently in use.

    for (size_t i = 0; i < pInitialMap->numEntries; ++i)
    {
        const DexprOS_InitialMemMapEntry* pEntry = &pInitialMap->pEntries[i];

        if (!ShouldCreateTreeForType(pEntry->memoryType))
            continue;

        switch (pEntry->usage)
        {
        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_CODE:
        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA:
        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_CODE:
        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_DATA:
        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_INITIAL_MEMMAP:
            
            {
                DexprOS_PhysicalMemoryAddress startAddress = pEntry->physicalAddress;
                DexprOS_PhysicalMemorySize size = pEntry->numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;

                MarkUsedTreePages(pTree,
                                  startAddress,
                                  size);
            }
            break;

        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE:
        case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_MAPPED:
            break;
        }
    }

    for (size_t i = 0; i < numOtherRegionsInUse; ++i)
    {
        MarkUsedTreePages(pTree,
                          pOtherRegionsInUse[i],
                          pOtherRegionsSizes[i]);
    }


    FillTreeAccelBlocks(pTree);
}


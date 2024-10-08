#include "DexprOS/Kernel/efi/PhysicalMemTreeGenEfi.h"

#include "DexprOS/Kernel/efi/EfiMemMapIteration.h"
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

    DexprOS_PhysMemTreeEfiSizeData* pSizeData = (DexprOS_PhysMemTreeEfiSizeData*)pUserData;
    pSizeData->numTreeEntries += 1;

    const size_t numPages = rangeSize / DEXPROS_PAGE_SIZE;

    size_t numBlocks = numPages;

    for (;;)
    {
        pSizeData->numTreeLevelStructs += 1;
        
        if (numBlocks > sizeof(uint8_t*))
            pSizeData->totalBlocksSize += numBlocks;

        if (numBlocks <= 1)
            return;

        numBlocks = (numBlocks + 1) / 2;
    }


    (void)physicalAddress;
    (void)memFlags;
}


DexprOS_PhysMemTreeEfiSizeData
DexprOS_GetPhysicalMemTreeSizeFromEfi(const void* pUefiMemoryMap,
                                      UINTN memoryMapSize,
                                      UINTN memoryDescriptorSize,
                                      UINTN memoryDescriptorVersion)
{
    DexprOS_PhysMemTreeEfiSizeData sizeData;
    sizeData.bufferAlignment = DEXPROS_FUNDAMENTAL_ALIGNMENT;
    sizeData.bufferSize = 0;
    sizeData.treeEntriesOffset = 0;
    sizeData.numTreeEntries = 0;
    sizeData.treeLevelStructsOffset = 0;
    sizeData.numTreeLevelStructs = 0;
    sizeData.blocksOffset = 0;
    sizeData.totalBlocksSize = 0;

    DexprOS_IterateEfiMemMap(pUefiMemoryMap,
                             memoryMapSize,
                             memoryDescriptorSize,
                             memoryDescriptorVersion,
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
    const DexprOS_PhysMemTreeEfiSizeData* pSizeData;

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
        for (size_t i = 0; i < sizeof(pLevel->blocks.inPlaceBlocks); ++i)
            pLevel->blocks.inPlaceBlocks[i] = 0;
    }

    return true;
}

static bool CreateTreeLevels(PhysMemTreeCreateData* pCreateData,
                             DexprOS_PhysicalMemTreeEntry* pEntry)
{
    size_t numBlocks = pEntry->numPages;

    // Calculate number of levels
    size_t numLevels = 0;
    for (;;)
    {
        numLevels += 1;

        if (numBlocks <= 1)
            break;

        numBlocks = (numBlocks + 1) / 2;
    }
    pEntry->numTreeLevels = numLevels;
    pCreateData->treeLevelStructIndex += numLevels;

    if (pCreateData->treeLevelStructIndex > pCreateData->pSizeData->numTreeLevelStructs)
        return false;


    // Calculate number of blocks for each level

    numBlocks = pEntry->numPages;


    for (size_t i = 0; i < numLevels; ++i)
    {
        size_t iLevel = numLevels - i - 1;

        DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[iLevel];
        pLevel->numBlocks = numBlocks;

        numBlocks = (numBlocks + 1) / 2;
    }


    // Create blocks inside levels

    for (size_t i = 0; i < numLevels; ++i)
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
    pEntry->numPages = rangeSize / DEXPROS_PAGE_SIZE;
    pEntry->numTreeLevels = 0;
    pEntry->pTreeLevels = &pCreateData->pLevels[pCreateData->treeLevelStructIndex];
    pEntry->pNextEntry = NULL;


    if (!CreateTreeLevels(pCreateData, pEntry))
    {
        pCreateData->success = false;
        return;
    }
}


bool DexprOS_CreatePhysicalMemTreeFromEfi(DexprOS_PhysicalMemTree* pResult,
                                          const void* pUefiMemoryMap,
                                          UINTN memoryMapSize,
                                          UINTN memoryDescriptorSize,
                                          UINTN memoryDescriptorVersion,
                                          void* pBuffer,
                                          const DexprOS_PhysMemTreeEfiSizeData* pSizeData)
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

    DexprOS_IterateEfiMemMap(pUefiMemoryMap,
                             memoryMapSize,
                             memoryDescriptorSize,
                             memoryDescriptorVersion,
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
        size_t iLastLevel = pEntry->numTreeLevels - 1;
        DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[iLastLevel];

        size_t numBlocks = pLevel->numBlocks;

        uint8_t* pBlocks = (numBlocks > sizeof(pLevel->blocks.inPlaceBlocks) ?
                            pLevel->blocks.pBlocks : pLevel->blocks.inPlaceBlocks);

        memset(pBlocks, DEXPROS_PHYS_MEM_PAGE_AVAILABLE_BIT, numBlocks);


        pEntry = pEntry->pNextEntry;
    }
}


static void MarkTreeEntryPagesUsed(const DexprOS_PhysicalMemTreeEntry* pEntry,
                                   size_t firstPageIndex,
                                   size_t numPages)
{
    size_t iLastLevel = pEntry->numTreeLevels - 1;
    DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[iLastLevel];

    size_t numBlocks = pLevel->numBlocks;

    uint8_t* pBlocks = (numBlocks > sizeof(pLevel->blocks.inPlaceBlocks) ?
                        pLevel->blocks.pBlocks : pLevel->blocks.inPlaceBlocks);

    memset(pBlocks + firstPageIndex, DEXPROS_PHYS_MEM_PAGE_UNAVAILABLE, numPages);
}

static void MarkTreeRegionPagesUsed(const DexprOS_PhysicalMemTree* pTree,
                                    DexprOS_PhysicalMemoryAddress regionStartAddress,
                                    DexprOS_PhysicalMemorySize regionSize)
{
    DexprOS_PhysicalMemoryAddress startAddress = (regionStartAddress / DEXPROS_PAGE_SIZE) *
                                                 DEXPROS_PAGE_SIZE;
    DexprOS_PhysicalMemoryAddress endAddress = DEXPROS_ALIGN(regionStartAddress + regionSize,
                                                             DEXPROS_PAGE_SIZE);


    DexprOS_PhysicalMemTreeEntry* pEntry = pTree->pFirstEntry;

    while (pEntry != NULL)
    {
        if (startAddress <= pEntry->endAddress &&
            endAddress >= pEntry->startAddress)
        {
            DexprOS_PhysicalMemoryAddress entryStart = (startAddress > pEntry->startAddress ?
                                                        startAddress : pEntry->startAddress);
            DexprOS_PhysicalMemoryAddress entryEnd = (endAddress < pEntry->endAddress ?
                                                      endAddress : pEntry->endAddress);
            size_t firstPageIndex = (entryStart - pEntry->startAddress) / DEXPROS_PAGE_SIZE;
            size_t numPages = (entryEnd - entryStart) / DEXPROS_PAGE_SIZE;

            MarkTreeEntryPagesUsed(pEntry, firstPageIndex, numPages);
        }

        pEntry = pEntry->pNextEntry;
    }
}


static void FillTreeLevelAccelBlocks(const DexprOS_PhysicalMemTreeEntry* pEntry,
                                     size_t iLevel)
{
    DexprOS_PhysicalMemTreeLevel* pLevel = &pEntry->pTreeLevels[iLevel];
    DexprOS_PhysicalMemTreeLevel* pNextLevel = &pEntry->pTreeLevels[iLevel + 1];

    size_t numBlocks = pLevel->numBlocks;
    size_t numNextBlocks = pNextLevel->numBlocks;
    
    uint8_t* pBlocks = (numBlocks > sizeof(pLevel->blocks.inPlaceBlocks) ?
                        pLevel->blocks.pBlocks : pLevel->blocks.inPlaceBlocks);

    uint8_t* pNextBlocks = (numNextBlocks > sizeof(pNextLevel->blocks.inPlaceBlocks) ?
                            pNextLevel->blocks.pBlocks : pNextLevel->blocks.inPlaceBlocks);


    for (size_t i = 0; i < numBlocks; ++i)
    {
        size_t iNext0 = i * 2;
        size_t iNext1 = i * 2 + 1;

        uint8_t state0 = 0;
        uint8_t state1 = 0;

        state0 = pNextBlocks[iNext0];
        if (iNext1 < numNextBlocks)
            state1 = pNextBlocks[iNext1];


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

void DexprOS_MarkPhysicalMemTreeRegionsFromEfi(const DexprOS_PhysicalMemTree* pTree,
                                               const void* pUefiMemoryMap,
                                               UINTN memoryMapSize,
                                               UINTN memoryDescriptorSize,
                                               UINTN memoryDescriptorVersion,
                                               size_t numOtherRegionsInUse,
                                               const DexprOS_PhysicalMemoryAddress* pOtherRegionsInUse,
                                               const DexprOS_PhysicalMemorySize* pOtherRegionsSizes)
{
    SetAllTreePagesAvailable(pTree);

    // Mark efi loader code and data regions on the map because they're included
    // in the OS's usable regions managed by the tree
    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)((char*)pUefiMemoryMap + memOffset);
        if (pMemoryDesc->Type == EfiLoaderCode || pMemoryDesc->Type == EfiLoaderData)
        {
            DexprOS_PhysicalMemoryAddress startAddress = pMemoryDesc->PhysicalStart;
            DexprOS_PhysicalMemorySize size = pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

            MarkTreeRegionPagesUsed(pTree,
                                    startAddress,
                                    size);
        }
    }

    for (size_t i = 0; i < numOtherRegionsInUse; ++i)
    {
        MarkTreeRegionPagesUsed(pTree,
                                pOtherRegionsInUse[i],
                                pOtherRegionsSizes[i]);
    }

    FillTreeAccelBlocks(pTree);

    (void)memoryDescriptorVersion;
}


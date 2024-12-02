#include "DexprOS/Kernel/Memory/PhysicalMemMapGen.h"
#include "DexprOS/Kernel/Memory/MemoryDef.h"


static void CountMemoryRanges(DexprOS_PhysicalMemoryAddress physicalAddress,
                              DexprOS_PhysicalMemorySize rangeSize,
                              DexprOS_PhysicalMemoryType memType,
                              DexprOS_PhysicalMemoryFlags memFlags,
                              void* pUserData)
{
    size_t* pNumEntries = (size_t*)pUserData;
    *pNumEntries += 1;

    (void)physicalAddress;
    (void)rangeSize;
    (void)memType;
    (void)memFlags;
}

void DexprOS_GetPhysicalMemMapSize(const DexprOS_InitialMemMap* pInitialMap,
                                   size_t* pOutBufferSize,
                                   size_t* pOutBufferAlignment)
{
    size_t numEntries = 0;

    DexprOS_IterateInitialMemMapCombinedPhysRanges(pInitialMap,
                                                   CountMemoryRanges,
                                                   (void*)&numEntries);

    *pOutBufferSize = numEntries * sizeof(DexprOS_PhysicalMemoryRange);
    *pOutBufferAlignment = DEXPROS_FUNDAMENTAL_ALIGNMENT;
}



typedef struct DexprOS_PhysMemMapFillData
{
    DexprOS_PhysicalMemoryRange* pEntries;
    size_t bufferSize;

    size_t currentIndex;
    bool success;
} PhysMemMapFillData;


static void FillMemMapEntry(DexprOS_PhysicalMemoryAddress physicalAddress,
                            DexprOS_PhysicalMemorySize rangeSize,
                            DexprOS_PhysicalMemoryType memType,
                            DexprOS_PhysicalMemoryFlags memFlags,
                            void* pUserData)
{
    PhysMemMapFillData* pFillData = (PhysMemMapFillData*)pUserData;

    DexprOS_PhysicalMemoryRange* pEntries = pFillData->pEntries;
    size_t index = pFillData->currentIndex;

    size_t currentSize = (index + 1) * sizeof(DexprOS_PhysicalMemoryRange);
    if (currentSize > pFillData->bufferSize)
    {
        pFillData->success = false;
        return;
    }

    pEntries[index].memoryType = memType;
    pEntries[index].physicalAddress = physicalAddress;
    pEntries[index].rangeSize = rangeSize;
    pEntries[index].flags = memFlags;

    pFillData->currentIndex += 1;
}

bool DexprOS_CreatePhysicalMemMap(DexprOS_PhysicalMemMap* pResult,
                                  const DexprOS_InitialMemMap* pInitialMap,
                                  void* pBuffer,
                                  size_t bufferSize)
{
    PhysMemMapFillData fillData;
    fillData.pEntries = (DexprOS_PhysicalMemoryRange*)pBuffer;
    fillData.bufferSize = bufferSize;
    fillData.currentIndex = 0;
    fillData.success = true;

    DexprOS_IterateInitialMemMapCombinedPhysRanges(pInitialMap,
                                                   FillMemMapEntry,
                                                   (void*)&fillData);

    pResult->numEntries = fillData.currentIndex;
    pResult->pEntries = fillData.pEntries;
    return fillData.success;
}


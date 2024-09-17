#include "DexprOS/Kernel/efi/PhysicalMemMapEfi.h"

#include "DexprOS/Kernel/efi/EfiMemMapIteration.h"
#include "DexprOS/Kernel/Memory/MemoryDef.h"


static void CountEfiMemoryRanges(DexprOS_PhysicalMemoryAddress physicalAddress,
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

void DexprOS_GetPhysicalMemMapSizeFromEfi(const void* pUefiMemoryMap,
                                          UINTN memoryMapSize,
                                          UINTN memoryDescriptorSize,
                                          UINTN memoryDescriptorVersion,
                                          size_t* pOutBufferSize,
                                          size_t* pOutBufferAlignment)
{
    size_t numEntries = 0;

    DexprOS_IterateEfiMemMap(pUefiMemoryMap,
                             memoryMapSize,
                             memoryDescriptorSize,
                             memoryDescriptorVersion,
                             CountEfiMemoryRanges,
                             (void*)&numEntries);

    *pOutBufferSize = numEntries * sizeof(DexprOS_PhysicalMemoryRange);
    *pOutBufferAlignment = DEXPROS_FUNDAMENTAL_ALIGNMENT;
}



typedef struct DexprOS_MemMapFromEfiFillData
{
    DexprOS_PhysicalMemoryRange* pEntries;
    size_t bufferSize;

    size_t currentIndex;
    bool success;
} MemMapFromEfiFillData;


static void FillMemMapEntry(DexprOS_PhysicalMemoryAddress physicalAddress,
                            DexprOS_PhysicalMemorySize rangeSize,
                            DexprOS_PhysicalMemoryType memType,
                            DexprOS_PhysicalMemoryFlags memFlags,
                            void* pUserData)
{
    MemMapFromEfiFillData* pFillData = (MemMapFromEfiFillData*)pUserData;

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

bool DexprOS_CreatePhysicalMemMapFromEfi(DexprOS_PhysicalMemMap* pResult,
                                         const void* pUefiMemoryMap,
                                         UINTN memoryMapSize,
                                         UINTN memoryDescriptorSize,
                                         UINTN memoryDescriptorVersion,
                                         void* pBuffer,
                                         size_t bufferSize)
{
    MemMapFromEfiFillData fillData;
    fillData.pEntries = (DexprOS_PhysicalMemoryRange*)pBuffer;
    fillData.bufferSize = bufferSize;
    fillData.currentIndex = 0;
    fillData.success = true;

    DexprOS_IterateEfiMemMap(pUefiMemoryMap,
                             memoryMapSize,
                             memoryDescriptorSize,
                             memoryDescriptorVersion,
                             FillMemMapEntry,
                             (void*)&fillData);

    pResult->numEntries = fillData.currentIndex;
    pResult->pEntries = fillData.pEntries;
    return fillData.success;
}


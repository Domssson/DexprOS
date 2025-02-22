#include "DexprOS/Kernel/Memory/InitialLinearAllocator.h"


DexprOS_InitialLinearAllocator DexprOS_InitInitialLinearAllocator(const DexprOS_InitialMemMap* pInitialMap)
{
    DexprOS_InitialLinearAllocator allocator;
    allocator.pInitialMap = pInitialMap;

    const size_t preferredLowestAddress = 0x1000000;

    size_t firstIndex = pInitialMap->numEntries;


    for (size_t i = 0; i < pInitialMap->numEntries; ++i)
    {
        if (pInitialMap->pEntries[i].memoryType != DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE ||
            pInitialMap->pEntries[i].usage != DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_AVAILABLE)
            continue;

        if (firstIndex == pInitialMap->numEntries)
            firstIndex = i;

        DexprOS_PhysicalMemoryAddress begAddress = pInitialMap->pEntries[i].physicalAddress;
        DexprOS_PhysicalMemoryAddress endAddress = begAddress +
                                                   pInitialMap->pEntries[i].numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;

        if (endAddress > preferredLowestAddress)
        {
            allocator.startAddress = begAddress;
            allocator.currentEntryIndex = i;
            allocator.usedEntryPages = 0;
            if (begAddress < preferredLowestAddress)
            {
                allocator.startAddress = preferredLowestAddress;
                allocator.usedEntryPages = (preferredLowestAddress - begAddress) / DEXPROS_PHYSICAL_PAGE_SIZE;
            }
            return allocator;
        }
    }

    allocator.startAddress = pInitialMap->pEntries[firstIndex].physicalAddress;
    allocator.currentEntryIndex = firstIndex;
    allocator.usedEntryPages = 0;
    return allocator;
}


bool DexprOS_InitialLinearPagesAlloc(DexprOS_InitialLinearAllocator* pAllocator,
                                     size_t numPages,
                                     DexprOS_PhysicalMemoryAddress* pOutPhys,
                                     DexprOS_VirtualMemoryAddress* pOutVirt)
{
    const DexprOS_InitialMemMap* pMemMap = pAllocator->pInitialMap;

    
    for (; pAllocator->currentEntryIndex < pMemMap->numEntries; pAllocator->currentEntryIndex += 1)
    {
        const DexprOS_InitialMemMapEntry* pEntry = &pMemMap->pEntries[pAllocator->currentEntryIndex];

        if (pEntry->memoryType != DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE ||
            pEntry->usage != DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_AVAILABLE)
            continue;

        if (pEntry->numPhysicalPages >= pAllocator->usedEntryPages + numPages)
        {
            DexprOS_PhysicalMemoryAddress physAddress = pEntry->physicalAddress +
                                                        pAllocator->usedEntryPages * DEXPROS_PHYSICAL_PAGE_SIZE;
            if (pOutPhys != NULL)
                *pOutPhys = physAddress;
            if (pOutVirt != NULL)
                *pOutVirt = physAddress + pMemMap->virtualMapOffset;
            
            pAllocator->usedEntryPages += numPages;

            if (pAllocator->usedEntryPages < pEntry->numPhysicalPages)
                return true;
            break;
        }

        pAllocator->usedEntryPages = 0;
    }

    if (pAllocator->currentEntryIndex >= pMemMap->numEntries)
        return false;


    pAllocator->currentEntryIndex += 1;
    pAllocator->usedEntryPages = 0;
    for (; pAllocator->currentEntryIndex < pMemMap->numEntries; pAllocator->currentEntryIndex += 1)
    {
        const DexprOS_InitialMemMapEntry* pEntry = &pMemMap->pEntries[pAllocator->currentEntryIndex];
        
        if (pEntry->memoryType == DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE &&
            pEntry->usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_AVAILABLE)
            break;
    }
    return true;
}


bool DexprOS_InitialLinearPageAlloc(DexprOS_InitialLinearAllocator* pAllocator,
                                    DexprOS_PhysicalMemoryAddress* pOutPhys,
                                    DexprOS_VirtualMemoryAddress* pOutVirt)
{
    const DexprOS_InitialMemMap* pMemMap = pAllocator->pInitialMap;

    if (pAllocator->currentEntryIndex >= pMemMap->numEntries)
        return false;

    
    const DexprOS_InitialMemMapEntry* pEntry = &pMemMap->pEntries[pAllocator->currentEntryIndex];

    DexprOS_PhysicalMemoryAddress physAddress = pEntry->physicalAddress +
                                                pAllocator->usedEntryPages * DEXPROS_PHYSICAL_PAGE_SIZE;

    if (pOutPhys != NULL)
        *pOutPhys = physAddress;
    if (pOutVirt != NULL)
        *pOutVirt = physAddress + pMemMap->virtualMapOffset;

    pAllocator->usedEntryPages += 1;
    if (pAllocator->usedEntryPages < pEntry->numPhysicalPages)
        return true;

    
    pAllocator->currentEntryIndex += 1;
    pAllocator->usedEntryPages = 0;
    for (; pAllocator->currentEntryIndex < pMemMap->numEntries; pAllocator->currentEntryIndex += 1)
    {
        pEntry = &pMemMap->pEntries[pAllocator->currentEntryIndex];
        
        if (pEntry->memoryType == DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE &&
            pEntry->usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_AVAILABLE)
            break;
    }
    return true;
}


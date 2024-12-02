#include "DexprOS/Kernel/Memory/InitialMemMap.h"


void DexprOS_IterateInitialMemMapCombinedPhysRanges(const DexprOS_InitialMemMap* pMemMap,
                                                    DexprOS_InitialMemMapPhysIterFunc pFunc,
                                                    void* pUserData)
{
    for (size_t i = 0; i < pMemMap->numEntries; ++i)
    {
        DexprOS_PhysicalMemoryAddress start = pMemMap->pEntries[i].physicalAddress;
        size_t numPages = pMemMap->pEntries[i].numPhysicalPages;
        DexprOS_PhysicalMemoryType memType = pMemMap->pEntries[i].memoryType;
        DexprOS_PhysicalMemoryFlags memFlags = pMemMap->pEntries[i].flags;

        DexprOS_PhysicalMemoryAddress rangeEnd = start + numPages * DEXPROS_PHYSICAL_PAGE_SIZE;

        while (i + 1 < pMemMap->numEntries)
        {
            if (pMemMap->pEntries[i + 1].memoryType == memType &&
                pMemMap->pEntries[i + 1].flags == memFlags &&
                pMemMap->pEntries[i + 1].physicalAddress == rangeEnd)
            {
                numPages += pMemMap->pEntries[i + 1].numPhysicalPages;
                rangeEnd += pMemMap->pEntries[i + 1].numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;
                i += 1;
            }
            else
                break;
        }

        pFunc(start, numPages * DEXPROS_PHYSICAL_PAGE_SIZE, memType, memFlags, pUserData);
    }
}

#include "DexprOS/Kernel/x86_64/StartupVirtMapCreator.h"

#include "DexprOS/Kernel/x86_64/StartupVirtMapSwitch.h"

#include "DexprOS/Kernel/x86_64/PagingSettings.h"

#include "DexprOS/Kernel/kstdlib/string.h"


#define DEXPROS_4_LEVEL_PAGEMAP_HALF_ADDRESS 0xFFFF800000000000
#define DEXPROS_5_LEVEL_PAGEMAP_HALF_ADDRESS 0xFF00000000000000


#define DEXPROS_DEFAULT_KERNEL_MAP_OFFSET 0x80000000


#define DEXPROS_STARTUP_RESERVED_STACK_SPACE 0x80000000 // 2 GiB

#define DEXPROS_DEFAULT_STACK_SIZE 0x40000 // 256 KiB


#define DEXPROS_NUM_PAGE_TABLE_ENTRIES 512


#define DEXPROS_PAGE_PRESENT_BIT 0x1
#define DEXPROS_PAGE_READWRITE_BIT 0x2
#define DEXPROS_PAGE_NOEXECUTE_BIT (UINT64_C(1) << 63)


#define DEXPROS_STARTUP_MAP_PRESENT_BIT 0x1
#define DEXPROS_STARTUP_MAP_WRITEABLE_BIT 0x2
#define DEXPROS_STARTUP_MAP_NOEXECUTE_BIT 0x4


static DexprOS_VirtualMemoryAddress GetHalfAddress(void)
{
    if (g_DexprOS_PagingSettings.pagingMode == DEXPROS_PAGING_MODE_5_LEVEL)
        return DEXPROS_5_LEVEL_PAGEMAP_HALF_ADDRESS;
    return DEXPROS_4_LEVEL_PAGEMAP_HALF_ADDRESS;
}


static bool IsRegionKernel(const DexprOS_InitialMemMapEntry* pEntry)
{
    return (pEntry->usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_CODE ||
            pEntry->usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA ||
            pEntry->usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_INITIAL_MEMMAP);
}


static bool IsRegionEfi(const DexprOS_InitialMemMapEntry* pEntry)
{
    switch (pEntry->memoryType)
    {
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA:
        return true;

    default:
        break;
    }

    if ((pEntry->flags & DEXPROS_PHYSICAL_MEMORY_EFI_RUNTIME_BIT) == DEXPROS_PHYSICAL_MEMORY_EFI_RUNTIME_BIT)
        return true;

    return false;
}


static bool IsRegionUsable(const DexprOS_InitialMemMapEntry* pEntry)
{
    return pEntry->memoryType == DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE;
}


static unsigned GetKernelRegionFlags(const DexprOS_InitialMemMapEntry* pEntry)
{
    unsigned flags = DEXPROS_STARTUP_MAP_PRESENT_BIT;

    if ((pEntry->flags & DEXPROS_PHYSICAL_MEMORY_READ_ONLY_BIT) == 0)
        flags |= DEXPROS_STARTUP_MAP_WRITEABLE_BIT;

    if (pEntry->usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA)
        flags |= DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;

    if ((pEntry->flags & DEXPROS_PHYSICAL_MEMORY_EXECUTE_PROTECTED_BIT) == DEXPROS_PHYSICAL_MEMORY_EXECUTE_PROTECTED_BIT)
        flags |= DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;
    
    return flags;
}

static unsigned GetEfiRegionFlags(const DexprOS_InitialMemMapEntry* pEntry)
{
    unsigned flags = DEXPROS_STARTUP_MAP_PRESENT_BIT;

    if ((pEntry->flags & DEXPROS_PHYSICAL_MEMORY_READ_ONLY_BIT) == 0)
        flags |= DEXPROS_STARTUP_MAP_WRITEABLE_BIT;

    if ((pEntry->flags & DEXPROS_PHYSICAL_MEMORY_EXECUTE_PROTECTED_BIT) == DEXPROS_PHYSICAL_MEMORY_EXECUTE_PROTECTED_BIT)
        flags |= DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;
    
    return flags;
}

static unsigned GetPageDirRegionFlags(void)
{
    return DEXPROS_STARTUP_MAP_WRITEABLE_BIT | DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;
}


static void CalcSpaceSizes(const DexprOS_InitialMemMap* pInitialMap,
                           size_t* pKernelSize,
                           size_t* pPageDirsSize,
                           size_t* pEfiSize)
{
    DexprOS_PhysicalMemoryAddress minKernelAddress = 0;
    DexprOS_PhysicalMemoryAddress maxKernelAddress = 0;
    DexprOS_PhysicalMemoryAddress minEfiAddress = 0;
    DexprOS_PhysicalMemoryAddress maxEfiAddress = 0;
    DexprOS_PhysicalMemoryAddress minPageDirAddress = 0;
    DexprOS_PhysicalMemoryAddress maxPageDirAddress = 0;

    for (size_t i = 0; i < pInitialMap->numEntries; ++i)
    {
        DexprOS_PhysicalMemoryAddress rangeStart = pInitialMap->pEntries[i].physicalAddress;
        DexprOS_PhysicalMemoryAddress rangeEnd = rangeStart +
            pInitialMap->pEntries[i].numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;
        

        if (IsRegionKernel(&pInitialMap->pEntries[i]))
        {
            if (rangeEnd > maxKernelAddress)
                maxKernelAddress = rangeEnd;
        }
        if (IsRegionEfi(&pInitialMap->pEntries[i]))
        {
            if (rangeEnd > maxEfiAddress)
                maxEfiAddress = rangeEnd;
        }
        if (IsRegionUsable(&pInitialMap->pEntries[i]))
        {
            if (rangeEnd > maxPageDirAddress)
                maxPageDirAddress = rangeEnd;
        }
    }

    *pKernelSize = maxKernelAddress - minKernelAddress;
    *pPageDirsSize = maxPageDirAddress - minPageDirAddress;
    *pEfiSize = maxEfiAddress - minEfiAddress;
}


static void GetSwitchFuncRegion(DexprOS_VirtualMemoryAddress* pOutSwitchStart,
                                 DexprOS_VirtualMemoryAddress* pOutSwitchEnd)
{
    DexprOS_VirtualMemoryAddress switchStart = (DexprOS_VirtualMemoryAddress)DexprOS_StartupVirtMapSwitchRegionStart;
    DexprOS_VirtualMemoryAddress switchEnd = (DexprOS_VirtualMemoryAddress)DexprOS_StartupVirtMapSwitchRegionEnd;
    switchStart -= switchStart % DEXPROS_PHYSICAL_PAGE_SIZE;
    switchEnd = ((switchEnd + DEXPROS_PHYSICAL_PAGE_SIZE - 1) / DEXPROS_PHYSICAL_PAGE_SIZE) * DEXPROS_PHYSICAL_PAGE_SIZE;

    *pOutSwitchStart = switchStart;
    *pOutSwitchEnd = switchEnd;
}


DexprOS_KernelMapping DexprOS_ChooseStartupVirtMapping(const DexprOS_InitialMemMap* pInitialMap,
                                                       size_t framebufferSize)
{
    DexprOS_KernelMapping mapping = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    mapping.kernelSpaceStart = GetHalfAddress();


    CalcSpaceSizes(pInitialMap,
                   &mapping.kernelMapSize,
                   &mapping.pageDirectoriesSize,
                   &mapping.efiSize);
    
    mapping.reservedStackSpace = DEXPROS_STARTUP_RESERVED_STACK_SPACE;
    mapping.framebufferSize = ((framebufferSize + DEXPROS_PHYSICAL_PAGE_SIZE - 1) / DEXPROS_PHYSICAL_PAGE_SIZE) *
                              DEXPROS_PHYSICAL_PAGE_SIZE;


    DexprOS_VirtualMemoryAddress switchFuncStart = 0;
    DexprOS_VirtualMemoryAddress switchFuncEnd = 0;
    GetSwitchFuncRegion(&switchFuncStart, &switchFuncEnd);


    mapping.kernelOffset = mapping.kernelSpaceStart + DEXPROS_DEFAULT_KERNEL_MAP_OFFSET;

    // Make sure the switch func range doesn't overlap with anything
    if (switchFuncEnd > mapping.kernelOffset && switchFuncStart < mapping.kernelOffset + mapping.kernelMapSize)
    {
        mapping.kernelOffset = ((switchFuncEnd + DEXPROS_DEFAULT_KERNEL_MAP_OFFSET - 1) /
                                 DEXPROS_DEFAULT_KERNEL_MAP_OFFSET) *
                                 DEXPROS_DEFAULT_KERNEL_MAP_OFFSET;
    }


    mapping.pageDirectoriesOffset = mapping.kernelOffset + mapping.kernelMapSize;

    if (switchFuncEnd > mapping.pageDirectoriesOffset &&
        switchFuncStart < mapping.pageDirectoriesOffset + mapping.pageDirectoriesSize)
    {
        mapping.pageDirectoriesOffset = switchFuncEnd;
    }
    
    mapping.pageDirectoriesOffset = ((mapping.pageDirectoriesOffset + DEXPROS_DEFAULT_KERNEL_MAP_OFFSET - 1) /
                                    DEXPROS_DEFAULT_KERNEL_MAP_OFFSET) *
                                    DEXPROS_DEFAULT_KERNEL_MAP_OFFSET;


    mapping.efiOffset = mapping.pageDirectoriesOffset + mapping.pageDirectoriesSize;
    if (switchFuncEnd > mapping.efiOffset && switchFuncStart < mapping.efiOffset + mapping.efiSize)
        mapping.efiOffset = switchFuncEnd;

    mapping.efiOffset = ((mapping.efiOffset + DEXPROS_DEFAULT_KERNEL_MAP_OFFSET - 1) /
                        DEXPROS_DEFAULT_KERNEL_MAP_OFFSET) *
                        DEXPROS_DEFAULT_KERNEL_MAP_OFFSET;


    mapping.stackOffset = mapping.efiOffset + mapping.efiSize;
    if (switchFuncEnd > mapping.stackOffset && switchFuncStart < mapping.stackOffset + mapping.reservedStackSpace)
        mapping.stackOffset = switchFuncEnd;
    
    mapping.stackOffset = ((mapping.stackOffset + DEXPROS_DEFAULT_KERNEL_MAP_OFFSET - 1) /
                          DEXPROS_DEFAULT_KERNEL_MAP_OFFSET) *
                          DEXPROS_DEFAULT_KERNEL_MAP_OFFSET;


    mapping.framebufferOffset = mapping.stackOffset + mapping.reservedStackSpace;
    if (switchFuncEnd > mapping.framebufferOffset && switchFuncStart < mapping.framebufferOffset + mapping.framebufferSize)
        mapping.framebufferOffset = switchFuncEnd;
    
    mapping.framebufferOffset = ((mapping.framebufferOffset + DEXPROS_DEFAULT_KERNEL_MAP_OFFSET - 1) /
                                DEXPROS_DEFAULT_KERNEL_MAP_OFFSET) *
                                DEXPROS_DEFAULT_KERNEL_MAP_OFFSET;


    return mapping;
}


static void FillLevel1PageTable(uint64_t* p1LevelTable,
                                DexprOS_VirtualMemoryAddress virtStart,
                                DexprOS_VirtualMemoryAddress virtEnd,
                                DexprOS_PhysicalMemoryAddress physStart,
                                unsigned flags)
{
    unsigned startIndex = (virtStart & 0x1FF000) >> 12;
    unsigned endIndex = ((virtEnd - 1) & 0x1FF000) >> 12;

    DexprOS_PhysicalMemoryAddress physAddress = physStart;

    for (unsigned j = startIndex; j <= endIndex; ++j)
    {
        uint64_t* pEntry = p1LevelTable + j;

        *pEntry = 0;
        *pEntry |= (physAddress & 0xFFFFFFFFFF000);

        if ((flags & DEXPROS_STARTUP_MAP_PRESENT_BIT) == DEXPROS_STARTUP_MAP_PRESENT_BIT)
            *pEntry |= DEXPROS_PAGE_PRESENT_BIT;

        if ((flags & DEXPROS_STARTUP_MAP_WRITEABLE_BIT) == DEXPROS_STARTUP_MAP_WRITEABLE_BIT)
            *pEntry |= DEXPROS_PAGE_READWRITE_BIT;

        if ((flags & DEXPROS_STARTUP_MAP_NOEXECUTE_BIT) == DEXPROS_STARTUP_MAP_NOEXECUTE_BIT)
        {
            if (g_DexprOS_PagingSettings.noExecuteAvailable)
                *pEntry |= DEXPROS_PAGE_NOEXECUTE_BIT;
        }

        physAddress += DEXPROS_PHYSICAL_PAGE_SIZE;
    }
}


static bool MapLevelPageTables(DexprOS_InitialLinearAllocator* pLinearAlloc,
                               uint64_t* pParentTable,
                               DexprOS_VirtualMemoryAddress virtStart,
                               DexprOS_VirtualMemoryAddress virtEnd,
                               DexprOS_PhysicalMemoryAddress physStart,
                               unsigned flags,
                               unsigned level)
{
    const unsigned levelMaskShifts[4] = {21, 30, 39, 48};
    const uint64_t levelManagedMemory[4] = {
        0x200000,
        0x40000000,
        0x8000000000,
        0x1000000000000
    };
    const uint64_t levelVirtMasks[4] = {
        0xFFFFFFFFFFE00000,
        0xFFFFFFFFC0000000,
        0xFFFFFF8000000000,
        0xFFFF000000000000
    };

    unsigned maskShift = levelMaskShifts[level - 1];
    uint64_t managedMemory = levelManagedMemory[level - 1];
    uint64_t virtMask = levelVirtMasks[level - 1];


    unsigned startIndex = (virtStart >> maskShift) & 0x1FF;
    unsigned endIndex = ((virtEnd - 1) >> maskShift) & 0x1FF;


    DexprOS_VirtualMemoryAddress subTableManagedStart = virtStart;


    for (unsigned j = startIndex; j <= endIndex; ++j)
    {
        uint64_t* pEntry = pParentTable + j;

        if ((*pEntry & DEXPROS_PAGE_PRESENT_BIT) == 0)
        {
            DexprOS_PhysicalMemoryAddress pNewTablePhys = 0;
            DexprOS_PhysicalMemoryAddress pNewTableVirt = 0;
            if (!DexprOS_InitialLinearPageAlloc(pLinearAlloc, &pNewTablePhys, &pNewTableVirt))
                return false;
            
            memset((void*)pNewTableVirt, 0, DEXPROS_PHYSICAL_PAGE_SIZE);
            *pEntry |= (pNewTablePhys & 0xFFFFFFFFFF000);
            *pEntry |= DEXPROS_PAGE_PRESENT_BIT;
        }

        if ((flags & DEXPROS_STARTUP_MAP_WRITEABLE_BIT) == DEXPROS_STARTUP_MAP_WRITEABLE_BIT)
            *pEntry |= DEXPROS_PAGE_READWRITE_BIT;


        DexprOS_PhysicalMemoryAddress newTableAddr = *pEntry & 0xFFFFFFFFFF000;

        uint64_t* pNewTable = (uint64_t*)(newTableAddr + pLinearAlloc->pInitialMap->virtualMapOffset);

        
        DexprOS_VirtualMemoryAddress newVirtStart = subTableManagedStart;
        DexprOS_VirtualMemoryAddress newVirtEnd = (subTableManagedStart & virtMask) + managedMemory;
        newVirtEnd = (newVirtEnd < virtEnd ? newVirtEnd : virtEnd);

        DexprOS_PhysicalMemoryAddress physAddress = physStart + (newVirtStart - virtStart);

        
        if (level == 1)
        {
            FillLevel1PageTable(pNewTable, newVirtStart, newVirtEnd, physAddress, flags);
        }
        else
        {
            if (!MapLevelPageTables(pLinearAlloc, pNewTable, newVirtStart, newVirtEnd, physAddress, flags, level - 1))
                return false;
        }

        subTableManagedStart = (subTableManagedStart & virtMask);
        subTableManagedStart += managedMemory;
    }
    return true;
}



static bool MarkPageTableUsed(uint64_t* pTopmostTable,
                              unsigned topmostTableDepth,
                              DexprOS_VirtualMemoryAddress targetTableAddr,
                              DexprOS_VirtualMemoryAddress currentVirtOffset)
{
    uint64_t* pTable = pTopmostTable;

    for (unsigned i = topmostTableDepth; i > 0; --i)
    {
        unsigned maskShift = 9 * (i - 1) + 12;

        unsigned tableOffset = (targetTableAddr >> maskShift) & 0x1FF;

        uint64_t* pEntry = pTable + tableOffset;

        if (i == 1)
        {
            *pEntry |= DEXPROS_PAGE_PRESENT_BIT;
            return true;
        }

        if ((*pEntry & DEXPROS_PAGE_PRESENT_BIT) == 0)
            return false;

        DexprOS_PhysicalMemoryAddress nextAddress = *pEntry & 0xFFFFFFFFFF000;
        pTable = (uint64_t*)(nextAddress + currentVirtOffset);
    }

    // Should never get here
    return false;
}


static bool SelfMapPageTable(uint64_t* pTopmostTable,
                             unsigned topmostTableDepth,
                             DexprOS_PhysicalMemoryAddress tablePhysAddr,
                             DexprOS_VirtualMemoryAddress currentVirtOffset,
                             DexprOS_VirtualMemoryAddress pageDirsOffset,
                             unsigned depth)
{
    if (!MarkPageTableUsed(pTopmostTable, topmostTableDepth,
                           tablePhysAddr + pageDirsOffset,
                           currentVirtOffset))
        return false;

    if (depth <= 1)
        return true;


    uint64_t* pTable = (uint64_t*)(tablePhysAddr + currentVirtOffset);


    for (unsigned i = 0; i < DEXPROS_NUM_PAGE_TABLE_ENTRIES; ++i)
    {
        uint64_t* pEntry = pTable + i;

        if ((*pEntry & DEXPROS_PAGE_PRESENT_BIT) == 0)
            continue;
        
        DexprOS_PhysicalMemoryAddress physAddr = *pEntry & 0xFFFFFFFFFF000;

        if (!SelfMapPageTable(pTopmostTable, topmostTableDepth,
                              physAddr,
                              currentVirtOffset,
                              pageDirsOffset,
                              depth - 1))
            return false;
    }

    return true;
}


static bool CreateBaseKernelPageTable(const DexprOS_InitialMemMap* pInitialMap,
                                      DexprOS_InitialLinearAllocator* pLinearAlloc,
                                      DexprOS_KernelStartInfo* pStartInfo,
                                      DexprOS_PhysicalMemoryAddress* pOutTable,
                                      DexprOS_PhysicalMemoryAddress stackAddress,
                                      DexprOS_PhysicalMemoryAddress dataBufStart,
                                      DexprOS_PhysicalMemoryAddress dataBufEnd,
                                      DexprOS_VirtualMemoryAddress switchCodeStart,
                                      DexprOS_VirtualMemoryAddress switchCodeEnd)
{
    DexprOS_KernelMapping* pKernelMapping = &pStartInfo->kernelMapping;


    unsigned numPagingLevels = 4;
    if (g_DexprOS_PagingSettings.pagingMode == DEXPROS_PAGING_MODE_5_LEVEL)
        numPagingLevels = 5;

    unsigned numSubLevels = numPagingLevels - 1;


    DexprOS_PhysicalMemoryAddress pTopmostTablePhys;
    DexprOS_PhysicalMemoryAddress pTopmostTableVirt;

    if (!DexprOS_InitialLinearPageAlloc(pLinearAlloc, &pTopmostTablePhys, &pTopmostTableVirt))
        return false;

    uint64_t* pTopmostTable = (uint64_t*)pTopmostTableVirt;
    memset(pTopmostTable, 0, DEXPROS_PHYSICAL_PAGE_SIZE);


    for (size_t i = 0; i < pInitialMap->numEntries; ++i)
    {
        const DexprOS_InitialMemMapEntry* pEntry = &pInitialMap->pEntries[i];
        DexprOS_PhysicalMemoryAddress start = pEntry->physicalAddress;
        DexprOS_PhysicalMemoryAddress end = start + pEntry->numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;


        if (IsRegionKernel(pEntry))
        {
            DexprOS_VirtualMemoryAddress offset = pKernelMapping->kernelOffset;
            unsigned flags = GetKernelRegionFlags(pEntry);

            if (!MapLevelPageTables(pLinearAlloc, pTopmostTable, start + offset, end + offset, start, flags, numSubLevels))
                return false;
        }

        if (IsRegionEfi(pEntry))
        {
            DexprOS_VirtualMemoryAddress offset = pKernelMapping->efiOffset;
            unsigned flags = GetEfiRegionFlags(pEntry);
            
            if (!MapLevelPageTables(pLinearAlloc, pTopmostTable, start + offset, end + offset, start, flags, numSubLevels))
                return false;
        }

        if (IsRegionUsable(pEntry))
        {
            DexprOS_VirtualMemoryAddress offset = pKernelMapping->pageDirectoriesOffset;
            unsigned flags = GetPageDirRegionFlags();

            if (!MapLevelPageTables(pLinearAlloc, pTopmostTable, start + offset, end + offset, start, flags, numSubLevels))
                return false;
        }
    }

    unsigned stackFlags = DEXPROS_STARTUP_MAP_PRESENT_BIT | DEXPROS_STARTUP_MAP_WRITEABLE_BIT | DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;
    if (!MapLevelPageTables(pLinearAlloc,
                            pTopmostTable,
                            pStartInfo->kernelStackBottom, pStartInfo->kernelStackTop,
                            stackAddress, stackFlags, numSubLevels))
        return false;
    

    DexprOS_PhysicalMemoryAddress framebufferPhysStart = pStartInfo->framebuffer.framebufferPhysAddr;
    framebufferPhysStart = (framebufferPhysStart / DEXPROS_PHYSICAL_PAGE_SIZE) * DEXPROS_PHYSICAL_PAGE_SIZE;

    unsigned framebufferFlags = DEXPROS_STARTUP_MAP_PRESENT_BIT | DEXPROS_STARTUP_MAP_WRITEABLE_BIT | DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;
    if (!MapLevelPageTables(pLinearAlloc,
                            pTopmostTable,
                            pStartInfo->framebufferStart, pStartInfo->framebufferEnd,
                            framebufferPhysStart, framebufferFlags, numSubLevels))
        return false;


    DexprOS_VirtualMemoryAddress dataBufOffset = pKernelMapping->kernelOffset;

    unsigned dataBufFlags = DEXPROS_STARTUP_MAP_PRESENT_BIT | DEXPROS_STARTUP_MAP_WRITEABLE_BIT | DEXPROS_STARTUP_MAP_NOEXECUTE_BIT;
    if (!MapLevelPageTables(pLinearAlloc,
                            pTopmostTable,
                            dataBufStart + dataBufOffset, dataBufEnd + dataBufOffset,
                            dataBufStart, dataBufFlags, numSubLevels))
        return false;

    
    DexprOS_PhysicalMemoryAddress switchPhysStart = switchCodeStart - pInitialMap->virtualMapOffset;

    unsigned switchFlags = DEXPROS_STARTUP_MAP_PRESENT_BIT;
    if (!MapLevelPageTables(pLinearAlloc,
                            pTopmostTable,
                            switchCodeStart, switchCodeEnd,
                            switchPhysStart, switchFlags, numSubLevels))
        return false;



    if (!SelfMapPageTable(pTopmostTable, numPagingLevels,
                          pTopmostTablePhys,
                          pInitialMap->virtualMapOffset,
                          pKernelMapping->pageDirectoriesOffset, numPagingLevels))
        return false;


    *pOutTable = pTopmostTablePhys;
    return true;
}


typedef struct DexprOS_StartupRemapDataBuffer
{
    DexprOS_KernelStartInfo startInfo;
    void (*entryFunc)(const DexprOS_KernelStartInfo*);
} StartupRemapDataBuf;


static bool AllocRemapDataBuf(DexprOS_InitialLinearAllocator* pLinearAlloc,
                              StartupRemapDataBuf** ppOutDataBuf,
                              DexprOS_PhysicalMemoryAddress* pOutPhysStart,
                              DexprOS_PhysicalMemoryAddress* pOutPhysEnd)
{
    size_t numPages = (sizeof(StartupRemapDataBuf) + DEXPROS_PHYSICAL_PAGE_SIZE - 1) /
                      DEXPROS_PHYSICAL_PAGE_SIZE;

    DexprOS_PhysicalMemoryAddress physAddr = 0;
    DexprOS_VirtualMemoryAddress virtAddr = 0;
    if (!DexprOS_InitialLinearPagesAlloc(pLinearAlloc, numPages, &physAddr, &virtAddr))
        return false;

    memset((void*)virtAddr, 0, DEXPROS_PHYSICAL_PAGE_SIZE * numPages);

    *ppOutDataBuf = (StartupRemapDataBuf*)virtAddr;
    *pOutPhysStart = physAddr;
    *pOutPhysEnd = physAddr + DEXPROS_PHYSICAL_PAGE_SIZE * numPages;
    return true;
}


static bool CreateKernelStack(DexprOS_InitialLinearAllocator* pLinearAlloc,
                              DexprOS_VirtualMemoryAddress currentVirtOffset,
                              DexprOS_PhysicalMemoryAddress* pOutStack)
{
    size_t numStackPages = DEXPROS_DEFAULT_STACK_SIZE / DEXPROS_PHYSICAL_PAGE_SIZE;

    DexprOS_PhysicalMemoryAddress stackMem = 0;
    if (!DexprOS_InitialLinearPagesAlloc(pLinearAlloc, numStackPages, &stackMem, NULL))
        return false;
    
    memset((void*)(stackMem + currentVirtOffset), 0, DEXPROS_DEFAULT_STACK_SIZE);
    *pOutStack = stackMem;

    return true;
}


static void AfterRemap(DexprOS_VirtualMemoryAddress dataBufAddress)
{
    StartupRemapDataBuf* pDataBuf = (StartupRemapDataBuf*)dataBufAddress;
    DexprOS_KernelStartInfo* pStartInfo = &pDataBuf->startInfo;


    DexprOS_VirtualMemoryAddress tmp = 0;

    tmp = (DexprOS_VirtualMemoryAddress)pDataBuf->entryFunc;
    tmp += pStartInfo->kernelMapping.kernelOffset;
    tmp -= pStartInfo->initialMemMap.virtualMapOffset;
    pDataBuf->entryFunc = (void (*)(const DexprOS_KernelStartInfo*))tmp;

    tmp = (DexprOS_VirtualMemoryAddress)pStartInfo->initialMemMap.pEntries;
    tmp += pStartInfo->kernelMapping.kernelOffset;
    tmp -= pStartInfo->initialMemMap.virtualMapOffset;
    pStartInfo->initialMemMap.pEntries = (DexprOS_InitialMemMapEntry*)tmp;

    tmp = (DexprOS_VirtualMemoryAddress)pStartInfo->pEfiSystemTable;
    tmp += pStartInfo->kernelMapping.efiOffset;
    tmp -= pStartInfo->initialMemMap.virtualMapOffset;
    pStartInfo->pEfiSystemTable = (EFI_SYSTEM_TABLE*)tmp;

    tmp = (DexprOS_VirtualMemoryAddress)pStartInfo->efiMemoryMap.pMemoryMapBuffer;
    // Efi mem map is actually stored in kernel's memory
    tmp += pStartInfo->kernelMapping.efiOffset;
    tmp -= pStartInfo->initialMemMap.virtualMapOffset;
    pStartInfo->efiMemoryMap.pMemoryMapBuffer = (void*)tmp;

    pStartInfo->framebuffer.framebufferVirtAddr = pStartInfo->kernelMapping.framebufferOffset;
    pStartInfo->framebuffer.framebufferVirtAddr += pStartInfo->framebuffer.framebufferPhysAddr % DEXPROS_PHYSICAL_PAGE_SIZE;

    tmp = (DexprOS_VirtualMemoryAddress)pStartInfo->framebuffer.pRenderBackBufferMemory;
    tmp += pStartInfo->kernelMapping.kernelOffset;
    tmp -= pStartInfo->initialMemMap.virtualMapOffset;
    pStartInfo->framebuffer.pRenderBackBufferMemory = (void*)tmp;

    
    pDataBuf->entryFunc(pStartInfo);
}


void DexprOS_PerformStartupRemap(const DexprOS_BaseStartupInfo* pBaseStartupInfo,
                                 const DexprOS_KernelMapping* pKernelMapping,
                                 void (*entryFunc)(const DexprOS_KernelStartInfo*))
{
    const DexprOS_InitialMemMap* pInitialMap = &pBaseStartupInfo->initialMemMap;

    DexprOS_InitialLinearAllocator linearAlloc = DexprOS_InitInitialLinearAllocator(pInitialMap);


    DexprOS_PhysicalMemoryAddress startInfoPhysStart = 0;
    DexprOS_PhysicalMemoryAddress startInfoPhysEnd = 0;
    StartupRemapDataBuf* pDataBuf = NULL;
    if (!AllocRemapDataBuf(&linearAlloc, &pDataBuf, &startInfoPhysStart, &startInfoPhysEnd))
        return;

    pDataBuf->entryFunc = entryFunc;

    DexprOS_KernelStartInfo* pStartInfo = &pDataBuf->startInfo;
    pStartInfo->kernelMapping = *pKernelMapping;

    pStartInfo->initialMemMap = pBaseStartupInfo->initialMemMap;

    pStartInfo->pEfiSystemTable = pBaseStartupInfo->pEfiSystemTable;
    pStartInfo->efiMemoryMap = pBaseStartupInfo->efiMemoryMap;

    pStartInfo->framebuffer = pBaseStartupInfo->framebuffer;

    pStartInfo->startInfoBufferStart = startInfoPhysStart + pKernelMapping->kernelOffset;
    pStartInfo->startInfoBufferEnd = startInfoPhysEnd + pKernelMapping->kernelOffset;


    DexprOS_PhysicalMemoryAddress stackMem = 0;
    if (!CreateKernelStack(&linearAlloc, pInitialMap->virtualMapOffset, &stackMem))
        return;

    pStartInfo->kernelStackTop = pKernelMapping->stackOffset + pKernelMapping->reservedStackSpace;
    pStartInfo->kernelStackBottom = pStartInfo->kernelStackTop - DEXPROS_DEFAULT_STACK_SIZE;


    DexprOS_VirtualMemoryAddress switchStart = 0;
    DexprOS_VirtualMemoryAddress switchEnd = 0;
    GetSwitchFuncRegion(&switchStart, &switchEnd);

    pStartInfo->remapCodeBufferStart = switchStart;
    pStartInfo->remapCodeBufferEnd = switchEnd;


    pStartInfo->framebufferStart = pKernelMapping->framebufferOffset;
    pStartInfo->framebufferEnd = pKernelMapping->framebufferOffset + pKernelMapping->framebufferSize;
    pStartInfo->framebufferEnd = ((pStartInfo->framebufferEnd + DEXPROS_PHYSICAL_PAGE_SIZE - 1) /
                                 DEXPROS_PHYSICAL_PAGE_SIZE) * DEXPROS_PHYSICAL_PAGE_SIZE;


    DexprOS_PhysicalMemoryAddress pageTableAddr = 0;
    if (!CreateBaseKernelPageTable(pInitialMap, &linearAlloc,
                                   pStartInfo, &pageTableAddr,
                                   stackMem,
                                   startInfoPhysStart, startInfoPhysEnd,
                                   switchStart, switchEnd))
        return;
    
    
    DexprOS_VirtualMemoryAddress switchFuncAddr = (DexprOS_VirtualMemoryAddress)DexprOS_StartupVirtMapSwitchRegionStart;
    DexprOS_StartupVirtMapSwitchFunc switchFunc = (DexprOS_StartupVirtMapSwitchFunc)switchFuncAddr;

    DexprOS_VirtualMemoryAddress callAddr = (DexprOS_VirtualMemoryAddress)AfterRemap + pKernelMapping->kernelOffset;
    DexprOS_VirtualMemoryAddress startInfoVirtAddr = startInfoPhysStart + pKernelMapping->kernelOffset;

    switchFunc(pageTableAddr, pStartInfo->kernelStackTop, callAddr, startInfoVirtAddr);
}


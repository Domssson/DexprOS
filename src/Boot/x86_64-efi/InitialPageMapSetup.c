#include "DexprOS/Boot/x86_64-efi/InitialPageMapSetup.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE 4096


#define DEXPROSBOOT_PAGE_PRESENT_BIT 1
#define DEXPROSBOOT_PAGE_READWRITE_BIT (1 << 1)
#define DEXPROSBOOT_PAGE_USERSUPERVISOR_BIT (1 << 2)
#define DEXPROSBOOT_PAGE_WRITE_THROUGH_BIT (1 << 3)
#define DEXPROSBOOT_PAGE_CACHE_DISABLE_BIT (1 << 4)
#define DEXPROSBOOT_PAGE_ACCESSED_BIT (1 << 5)
#define DEXPROSBOOT_PAGE_EXECUTE_DISABLE_BIT (1 << 63)

#define DEXPROSBOOT_PAGE_DIRECTORY_PAGE_SIZE_BIT (1 << 7)

#define DXEPROSBOOT_PAGE_TABLE_DIRTY_BIT (1 << 6)
#define DEXPROSBOOT_PAGE_TABLE_PAT_BIT (1 << 7)
#define DEXPROSBOOT_PAGE_TABLE_GLOBAL_BIT (1 << 8)


// 2 MiB, 4096 * 512 B
#define DEXPROSBOOT_LEVEL1_PAGE_TABLE_MANAGED_BYTES 0x200000
// 1 GiB, 4096 * 512^2 B
#define DEXPROSBOOT_LEVEL2_PAGE_TABLE_MANAGED_BYTES 0x40000000
// 512 GiB, 4096 * 512^3 B
#define DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES 0x8000000000
// 256 TiB, 4096 * 512^4 B
#define DEXPROSBOOT_LEVEL4_PAGE_TABLE_MANAGED_BYTES 0x1000000000000


inline static bool ShouldIncludeInPageMap(EFI_MEMORY_TYPE type, UINT64 attributes)
{
    switch (type)
    {
    case EfiLoaderCode:
    case EfiLoaderData:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiConventionalMemory:
        return true;
    
    default:
        if ((attributes & EFI_MEMORY_RUNTIME) == EFI_MEMORY_RUNTIME)
            return true;
        return false;
    }
}

inline static bool ShouldIncludeInTransitionalPageMap(EFI_MEMORY_TYPE type, UINT64 attributes)
{
    switch (type)
    {
    case EfiLoaderCode:
    case EfiLoaderData:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiBootServicesCode:
    case EfiBootServicesData:
        return true;
    
    default:
        if ((attributes & EFI_MEMORY_RUNTIME) == EFI_MEMORY_RUNTIME)
            return true;
        return false;
    }
}


// Finds next lowest memory range suitable for including in a page map
static bool FindNextMemoryRange(EFI_PHYSICAL_ADDRESS minAddress,
                                const void* pUefiMemoryMap,
                                UINTN memoryMapSize,
                                UINTN memoryDescriptorSize,
                                UINTN memoryDescriptorVersion,
                                EFI_PHYSICAL_ADDRESS framebufferBase,
                                UINTN framebufferSize,
                                EFI_PHYSICAL_ADDRESS* pOutMemoryRangeBase,
                                EFI_PHYSICAL_ADDRESS* pOutMemoryRangeLast)
{
    (void)memoryDescriptorVersion;


    EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
    EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;
    bool memoryRangeFound = false;

    // Find the next lowest continuous memory region to process
    const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

        // First, we want to map only the memory ranges that are crucial
        // for the bootloader to function. The rest of them will be mapped later.
        if (!ShouldIncludeInPageMap(pMemoryDesc->Type, pMemoryDesc->Attribute))
            continue;


        EFI_PHYSICAL_ADDRESS mapMin = pMemoryDesc->PhysicalStart;
        EFI_PHYSICAL_ADDRESS mapMax = mapMin + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

        if (mapMin >= minAddress && (mapMin < memoryRangeBase || !memoryRangeFound))
        {
            memoryRangeBase = mapMin;
            memoryRangeLast = mapMax;
            memoryRangeFound = true;
        }
    }
    if (framebufferBase >= minAddress && (framebufferBase < memoryRangeLast || !memoryRangeFound))
    {
        memoryRangeBase = framebufferBase;
        memoryRangeLast = framebufferBase + framebufferSize;
        memoryRangeFound = true;
    }

    if (memoryRangeFound)
    {
        *pOutMemoryRangeBase = memoryRangeBase;
        *pOutMemoryRangeLast = memoryRangeLast;
        return true;
    }
    return false;
}


// Finds next lowest memory range suitable for including in a transitional page map
static bool FindNextMemoryRangeTransitional(EFI_PHYSICAL_ADDRESS minAddress,
                                            const void* pUefiMemoryMap,
                                            UINTN memoryMapSize,
                                            UINTN memoryDescriptorSize,
                                            UINTN memoryDescriptorVersion,
                                            EFI_PHYSICAL_ADDRESS* pOutMemoryRangeBase,
                                            EFI_PHYSICAL_ADDRESS* pOutMemoryRangeLast)
{
    (void)memoryDescriptorVersion;


    EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
    EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;
    bool memoryRangeFound = false;

    // Find the next lowest continuous memory region to process
    const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

        // First, we want to map only the memory ranges that are needed to
        // be able to switch to 5-level page map just a moment later.
        if (!ShouldIncludeInTransitionalPageMap(pMemoryDesc->Type, pMemoryDesc->Attribute))
            continue;


        EFI_PHYSICAL_ADDRESS mapMin = pMemoryDesc->PhysicalStart;
        EFI_PHYSICAL_ADDRESS mapMax = mapMin + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

        if (mapMin >= minAddress && (mapMin < memoryRangeBase || !memoryRangeFound))
        {
            memoryRangeBase = mapMin;
            memoryRangeLast = mapMax;
            memoryRangeFound = true;
        }
    }

    if (memoryRangeFound)
    {
        *pOutMemoryRangeBase = memoryRangeBase;
        *pOutMemoryRangeLast = memoryRangeLast;
        return true;
    }
    return false;
}


UINTN DexprOSBoot_CalculateInitialPageMap4Size(const void* pUefiMemoryMap,
                                               UINTN memoryMapSize,
                                               UINTN memoryDescriptorSize,
                                               UINTN memoryDescriptorVersion,
                                               EFI_PHYSICAL_ADDRESS framebufferBase,
                                               UINTN framebufferSize)
{
    const UINTN managedMemorySizePerTableLevel[3] = {
        DEXPROSBOOT_LEVEL1_PAGE_TABLE_MANAGED_BYTES, // 2 MiB, 4096 * 512 B
        DEXPROSBOOT_LEVEL2_PAGE_TABLE_MANAGED_BYTES, // 1 GiB, 4096 * 512^2 B
        DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES // 512 GiB, 4096 * 512^3 B
    };

    UINTN numTablesPerLevel[3] = {
        0, 0, 0
    };




    EFI_PHYSICAL_ADDRESS highestResolvedAddress = 0;

    UINTN highestTableIndices[3] = {
        0, 0, 0
    };


    for (;;)
    {
        EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
        EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;

        if (!FindNextMemoryRange(highestResolvedAddress,
                                 pUefiMemoryMap,
                                 memoryMapSize,
                                 memoryDescriptorSize,
                                 memoryDescriptorVersion,
                                 framebufferBase,
                                 framebufferSize,
                                 &memoryRangeBase,
                                 &memoryRangeLast))
            break;

        // Count number of tables per level
        for (int iLevel = 0; iLevel < 3; ++iLevel)
        {
            UINTN tableManSize = managedMemorySizePerTableLevel[iLevel];

            UINTN startTableIndex = (memoryRangeBase / tableManSize);
            UINTN endTableIndex = ((memoryRangeLast - 1) / tableManSize);

            UINTN numRangeTables = endTableIndex - startTableIndex;

            if (highestTableIndices[iLevel] < startTableIndex || numTablesPerLevel[iLevel] == 0)
                numRangeTables += 1;

            highestTableIndices[iLevel] = endTableIndex;
            numTablesPerLevel[iLevel] += numRangeTables;
        }

        highestResolvedAddress = memoryRangeLast;
    }


    // Add 1 as this is the only one PML4 table
    UINTN numAllTables = numTablesPerLevel[0] + numTablesPerLevel[1] + numTablesPerLevel[2] + 1;

    UINTN allocationSize = numAllTables * DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE;
    return allocationSize;
}


UINTN DexprOSBoot_CalculateTransitionalPageMap4Size(const void* pUefiMemoryMap,
                                                    UINTN memoryMapSize,
                                                    UINTN memoryDescriptorSize,
                                                    UINTN memoryDescriptorVersion)
{
    const UINTN managedMemorySizePerTableLevel[3] = {
        DEXPROSBOOT_LEVEL1_PAGE_TABLE_MANAGED_BYTES, // 2 MiB, 4096 * 512 B
        DEXPROSBOOT_LEVEL2_PAGE_TABLE_MANAGED_BYTES, // 1 GiB, 4096 * 512^2 B
        DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES // 512 GiB, 4096 * 512^3 B
    };

    UINTN numTablesPerLevel[3] = {
        0, 0, 0
    };




    EFI_PHYSICAL_ADDRESS highestResolvedAddress = 0;

    UINTN highestTableIndices[3] = {
        0, 0, 0
    };


    for (;;)
    {
        EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
        EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;
        
        if (!FindNextMemoryRangeTransitional(highestResolvedAddress,
                                             pUefiMemoryMap,
                                             memoryMapSize,
                                             memoryDescriptorSize,
                                             memoryDescriptorVersion,
                                             &memoryRangeBase,
                                             &memoryRangeLast))
            break;

        // Count number of tables per level
        for (int iLevel = 0; iLevel < 3; ++iLevel)
        {
            UINTN tableManSize = managedMemorySizePerTableLevel[iLevel];

            UINTN startTableIndex = (memoryRangeBase / tableManSize);
            UINTN endTableIndex = ((memoryRangeLast - 1) / tableManSize);

            UINTN numRangeTables = endTableIndex - startTableIndex;

            if (highestTableIndices[iLevel] < startTableIndex || numTablesPerLevel[iLevel] == 0)
                numRangeTables += 1;

            highestTableIndices[iLevel] = endTableIndex;
            numTablesPerLevel[iLevel] += numRangeTables;
        }

        highestResolvedAddress = memoryRangeLast;
    }


    // Add 1 as this is the only one PML4 table
    UINTN numAllTables = numTablesPerLevel[0] + numTablesPerLevel[1] + numTablesPerLevel[2] + 1;

    UINTN allocationSize = numAllTables * DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE;
    return allocationSize;
}


UINTN DexprOSBoot_CalculateInitialPageMap5Size(const void* pUefiMemoryMap,
                                               UINTN memoryMapSize,
                                               UINTN memoryDescriptorSize,
                                               UINTN memoryDescriptorVersion,
                                               EFI_PHYSICAL_ADDRESS framebufferBase,
                                               UINTN framebufferSize)
{
    const UINTN managedMemorySizePerTableLevel[4] = {
        DEXPROSBOOT_LEVEL1_PAGE_TABLE_MANAGED_BYTES, // 2 MiB, 4096 * 512 B
        DEXPROSBOOT_LEVEL2_PAGE_TABLE_MANAGED_BYTES, // 1 GiB, 4096 * 512^2 B
        DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES, // 512 GiB, 4096 * 512^3 B
        DEXPROSBOOT_LEVEL4_PAGE_TABLE_MANAGED_BYTES // 256 TiB, 4096 * 512^4 B
    };

    UINTN numTablesPerLevel[4] = {
        0, 0, 0, 0
    };



    EFI_PHYSICAL_ADDRESS highestResolvedAddress = 0;

    UINTN highestTableIndices[4] = {
        0, 0, 0, 0
    };

    for (;;)
    {
        EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
        EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;

        if (!FindNextMemoryRange(highestResolvedAddress,
                                 pUefiMemoryMap,
                                 memoryMapSize,
                                 memoryDescriptorSize,
                                 memoryDescriptorVersion,
                                 framebufferBase,
                                 framebufferSize,
                                 &memoryRangeBase,
                                 &memoryRangeLast))
            break;
        
        // Count number of tables per level
        for (int iLevel = 0; iLevel < 4; ++iLevel)
        {
            UINTN tableManSize = managedMemorySizePerTableLevel[iLevel];

            UINTN startTableIndex = (memoryRangeBase / tableManSize);
            UINTN endTableIndex = ((memoryRangeLast - 1) / tableManSize);

            UINTN numRangeTables = endTableIndex - startTableIndex;
            if (highestTableIndices[iLevel] < startTableIndex || numTablesPerLevel[iLevel] == 0)
                numRangeTables += 1;

            highestTableIndices[iLevel] = endTableIndex;
            numTablesPerLevel[iLevel] += numRangeTables;
        }

        highestResolvedAddress = memoryRangeLast;
    }


    // Add 1 as this is the only one PML5 table
    UINTN numAllTables = numTablesPerLevel[0] + numTablesPerLevel[1] + numTablesPerLevel[2] +
                         numTablesPerLevel[3] + 1;

    UINTN allocationSize = numAllTables * DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE;
    return allocationSize;
}



static void PrepareLevel1Table(EFI_PHYSICAL_ADDRESS startAddress,
                               EFI_PHYSICAL_ADDRESS endAddress,
                               uint64_t* p1LevelTable)
{
    unsigned startIndex = (startAddress & 0x1FF000) >> 12;
    unsigned endIndex = ((endAddress - 1) & 0x1FF000) >> 12;

    EFI_PHYSICAL_ADDRESS address = startAddress;

    for (unsigned j = startIndex; j <= endIndex; ++j)
    {
        uint64_t* pEntry = p1LevelTable + j;

        *pEntry = 0;
        *pEntry |= ((uint64_t)address & 0xFFFFFFFFFF000);
        *pEntry |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);

        address += DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE;
    }
}


static bool PrepareLevelPageTables(EFI_PHYSICAL_ADDRESS startAddress,
                                   EFI_PHYSICAL_ADDRESS endAddress,
                                   uint64_t* pParentTable,
                                   uint64_t** ppNextAvailableTableSpace,
                                   uint64_t* pPageMapBufferEnd,
                                   unsigned level)
{
    const unsigned levelMaskShifts[4] = {21, 30, 39, 48};
    const uint64_t levelManagedMemory[4] = {
        DEXPROSBOOT_LEVEL1_PAGE_TABLE_MANAGED_BYTES,
        DEXPROSBOOT_LEVEL2_PAGE_TABLE_MANAGED_BYTES,
        DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES,
        DEXPROSBOOT_LEVEL4_PAGE_TABLE_MANAGED_BYTES
    };
    const uint64_t levelMasks[4] = {
        0xFFFFFFFFFFE00000,
        0xFFFFFFFFC0000000,
        0xFFFFFF8000000000,
        0xFFFF000000000000
    };

    unsigned maskShift = levelMaskShifts[level - 1];
    uint64_t managedMemory = levelManagedMemory[level - 1];
    uint64_t levelMask = levelMasks[level - 1];


    unsigned startIndex = (startAddress >> maskShift) & 0x1FF;
    unsigned endIndex = ((endAddress - 1) >> maskShift) & 0x1FF;


    EFI_PHYSICAL_ADDRESS subTableManagedStart = startAddress;


    for (unsigned j = startIndex; j <= endIndex; ++j)
    {
        uint64_t* pEntry = pParentTable + j;

        if ((*pEntry & DEXPROSBOOT_PAGE_PRESENT_BIT) == 0)
        {
            if (*ppNextAvailableTableSpace + 512 > pPageMapBufferEnd)
                return false;
            
            uint64_t* pNewTable = *ppNextAvailableTableSpace;
            *ppNextAvailableTableSpace += 512;

            for (int i = 0; i < 512; ++i)
                pNewTable[i] = 0;
            
            *pEntry |= ((uint64_t)pNewTable & 0xFFFFFFFFFF000);
            *pEntry |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);
        }

        uint64_t* pNewTable = (uint64_t*)(*pEntry & 0xFFFFFFFFFF000);

        EFI_PHYSICAL_ADDRESS newStart = subTableManagedStart;
        EFI_PHYSICAL_ADDRESS newEnd = (subTableManagedStart & levelMask) + managedMemory;
        newEnd = (newEnd < endAddress ? newEnd : endAddress);

        
        if (level == 1)
        {
            PrepareLevel1Table(newStart, newEnd, pNewTable);
        }
        else
        {
            if (!PrepareLevelPageTables(newStart,
                                        newEnd,
                                        pNewTable,
                                        ppNextAvailableTableSpace,
                                        pPageMapBufferEnd,
                                        level - 1))
                return false;
        }

        subTableManagedStart = (subTableManagedStart & levelMask);
        subTableManagedStart += managedMemory;
    }
    return true;
}


static bool MapMemoryRangeInLevel4Table(EFI_PHYSICAL_ADDRESS startAddress,
                                        EFI_PHYSICAL_ADDRESS endAddress,
                                        uint64_t* pPML4Table,
                                        uint64_t** ppNextAvailableTableSpace,
                                        uint64_t* pPageMapBufferEnd)
{
    return PrepareLevelPageTables(startAddress, endAddress,
                                  pPML4Table,
                                  ppNextAvailableTableSpace,
                                  pPageMapBufferEnd,
                                  3);
}


static bool MapMemoryRangeInLevel5Table(EFI_PHYSICAL_ADDRESS startAddress,
                                        EFI_PHYSICAL_ADDRESS endAddress,
                                        uint64_t* pPML5Table,
                                        uint64_t** ppNextAvailableTableSpace,
                                        uint64_t* pPageMapBufferEnd)
{
    return PrepareLevelPageTables(startAddress, endAddress,
                                  pPML5Table,
                                  ppNextAvailableTableSpace,
                                  pPageMapBufferEnd,
                                  4);
}


void* DexprOSBoot_SetupInitialPageMap4(const void* pUefiMemoryMap,
                                       UINTN memoryMapSize,
                                       UINTN memoryDescriptorSize,
                                       UINTN memoryDescriptorVersion,
                                       EFI_PHYSICAL_ADDRESS framebufferBase,
                                       UINTN framebufferSize,
                                       void* pPageMapBuffer,
                                       UINTN pageMapBufferSize)
{
    if (((size_t)pPageMapBuffer % DEXPROSBOOT_INITIAL_PAGE_SIZE) != 0)
        return NULL;
    if (pageMapBufferSize < DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE)
        return NULL;


    uint64_t* pPageMapTables = (uint64_t*)pPageMapBuffer;

    uint64_t* pPML4Table = pPageMapTables;
    // Zero the 4-level table
    for (int i = 0; i < 512; ++i)
        pPML4Table[i] = 0;

    uint64_t* pNextAvailableTableSpace = pPageMapTables + 512;
    uint64_t* pPageMapBufferEnd = pPageMapTables + (pageMapBufferSize / sizeof(uint64_t));


    const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

        // First, we want to map only the memory ranges that are crucial
        // for the bootloader to function. The rest of them will be mapped later.
        if (!ShouldIncludeInPageMap(pMemoryDesc->Type, pMemoryDesc->Attribute))
            continue;
        

        EFI_PHYSICAL_ADDRESS startAddress = pMemoryDesc->PhysicalStart;
        EFI_PHYSICAL_ADDRESS endAddress = pMemoryDesc->PhysicalStart + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

        if (!MapMemoryRangeInLevel4Table(startAddress,
                                         endAddress,
                                         pPML4Table,
                                         &pNextAvailableTableSpace,
                                         pPageMapBufferEnd))
            return NULL;
    }

    // Map the framebuffer
    if (!MapMemoryRangeInLevel4Table(framebufferBase,
                                     framebufferBase + framebufferSize,
                                     pPML4Table,
                                     &pNextAvailableTableSpace,
                                     pPageMapBufferEnd))
        return NULL;


    (void)memoryDescriptorVersion;

    return pPML4Table;
}


void* DexprOSBoot_SetupTransitionalPageMap4(const void* pUefiMemoryMap,
                                            UINTN memoryMapSize,
                                            UINTN memoryDescriptorSize,
                                            UINTN memoryDescriptorVersion,
                                            void* pPageMapBuffer,
                                            UINTN pageMapBufferSize)
{
    if (((size_t)pPageMapBuffer % DEXPROSBOOT_INITIAL_PAGE_SIZE) != 0)
        return NULL;
    if (pageMapBufferSize < DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE)
        return NULL;


    uint64_t* pPageMapTables = (uint64_t*)pPageMapBuffer;

    uint64_t* pPML4Table = pPageMapTables;
    // Zero the 4-level table
    for (int i = 0; i < 512; ++i)
        pPML4Table[i] = 0;

    uint64_t* pNextAvailableTableSpace = pPageMapTables + 512;
    uint64_t* pPageMapBufferEnd = pPageMapTables + (pageMapBufferSize / sizeof(uint64_t));


    const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

        // First, we want to map only the memory ranges that are needed to
        // be able to switch to 5-level page map just a moment later.
        if (!ShouldIncludeInTransitionalPageMap(pMemoryDesc->Type, pMemoryDesc->Attribute))
            continue;
        

        EFI_PHYSICAL_ADDRESS startAddress = pMemoryDesc->PhysicalStart;
        EFI_PHYSICAL_ADDRESS endAddress = pMemoryDesc->PhysicalStart + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

        if (!MapMemoryRangeInLevel4Table(startAddress,
                                         endAddress,
                                         pPML4Table,
                                         &pNextAvailableTableSpace,
                                         pPageMapBufferEnd))
            return NULL;
    }


    (void)memoryDescriptorVersion;

    return pPML4Table;
}


void* DexprOSBoot_SetupInitialPageMap5(const void* pUefiMemoryMap,
                                       UINTN memoryMapSize,
                                       UINTN memoryDescriptorSize,
                                       UINTN memoryDescriptorVersion,
                                       EFI_PHYSICAL_ADDRESS framebufferBase,
                                       UINTN framebufferSize,
                                       void* pPageMapBuffer,
                                       UINTN pageMapBufferSize)
{
    if (((size_t)pPageMapBuffer % DEXPROSBOOT_INITIAL_PAGE_SIZE) != 0)
        return NULL;
    if (pageMapBufferSize < DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE)
        return NULL;


    uint64_t* pPageMapTables = (uint64_t*)pPageMapBuffer;

    uint64_t* pPML5Table = pPageMapTables;
    // Zero the 5-level table
    for (int i = 0; i < 512; ++i)
        pPML5Table[i] = 0;

    uint64_t* pNextAvailableTableSpace = pPageMapTables + 512;
    uint64_t* pPageMapBufferEnd = pPageMapTables + (pageMapBufferSize / sizeof(uint64_t));


    const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

        // First, we want to map only the memory ranges that are crucial
        // for the bootloader to function. The rest of them will be mapped later.
        if (!ShouldIncludeInPageMap(pMemoryDesc->Type, pMemoryDesc->Attribute))
            continue;
        

        EFI_PHYSICAL_ADDRESS startAddress = pMemoryDesc->PhysicalStart;
        EFI_PHYSICAL_ADDRESS endAddress = pMemoryDesc->PhysicalStart + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

        if (!MapMemoryRangeInLevel5Table(startAddress,
                                         endAddress,
                                         pPML5Table,
                                         &pNextAvailableTableSpace,
                                         pPageMapBufferEnd))
            return NULL;
    }

    // Map the framebuffer
    if (!MapMemoryRangeInLevel5Table(framebufferBase,
                                     framebufferBase + framebufferSize,
                                     pPML5Table,
                                     &pNextAvailableTableSpace,
                                     pPageMapBufferEnd))
        return NULL;


    (void)memoryDescriptorVersion;

    return pPML5Table;
}


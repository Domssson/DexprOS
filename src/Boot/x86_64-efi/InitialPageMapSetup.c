#include "DexprOS/Boot/x86_64-efi/InitialPageMapSetup.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE 4096
#define DEXPROSBOOT_PAGE_SIZE 4096


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


inline static bool ShouldIncludeMemoryTypeInPageMap(EFI_MEMORY_TYPE type)
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
        return false;
    }
}


UINTN DexprOSBoot_CalculatePageMap4SizeForLoader(const void* pUefiMemoryMap,
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
        1, 1, 1
    };




    EFI_PHYSICAL_ADDRESS highestResolvedAddress = 0;

    EFI_PHYSICAL_ADDRESS highestTableIndices[3] = {
        0, 0, 0
    };


    for (;;)
    {
        EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
        EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;

        // Find the next lowest continuous memory region to process
        const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
        for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
        {
            const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

            // First, we want to map only the memory ranges that are crucial
            // for the bootloader to function. The rest of them will be mapped later.
            if (!ShouldIncludeMemoryTypeInPageMap(pMemoryDesc->Type))
                continue;


            EFI_PHYSICAL_ADDRESS descStart = pMemoryDesc->PhysicalStart;
            EFI_PHYSICAL_ADDRESS mapMin = descStart > highestResolvedAddress ? descStart : highestResolvedAddress;
            EFI_PHYSICAL_ADDRESS mapMax = descStart + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

            if (mapMax > highestResolvedAddress)
            {
                if (memoryRangeLast == 0)
                {
                    memoryRangeBase = mapMin;
                    memoryRangeLast = mapMax;
                }
                else if (mapMin < memoryRangeBase)
                {
                    memoryRangeBase = mapMin;
                    memoryRangeLast = mapMax;
                }
                else if (mapMin == memoryRangeBase && mapMax > memoryRangeLast)
                {
                    memoryRangeLast = mapMax;
                }
            }
        }
        if (framebufferBase + framebufferSize > highestResolvedAddress)
        {
            EFI_PHYSICAL_ADDRESS mapMin = framebufferBase > highestResolvedAddress ? framebufferBase : highestResolvedAddress;
            EFI_PHYSICAL_ADDRESS mapMax = framebufferBase + framebufferSize;

            if (memoryRangeLast == 0)
            {
                memoryRangeBase = mapMin;
                memoryRangeLast = mapMax;
            }
            else if (mapMin < memoryRangeBase)
            {
                memoryRangeBase = mapMin;
                memoryRangeLast = mapMax;
            }
            else if (mapMin == memoryRangeBase && mapMax > memoryRangeLast)
            {
                memoryRangeLast = mapMax;
            }
        }


        // No memory ranges left
        if (memoryRangeLast == 0)
            break;

        // Count additional table levels
        for (int iLevel = 0; iLevel < 3; ++iLevel)
        {
            UINTN tableManSize = managedMemorySizePerTableLevel[iLevel];

            UINTN startTableIndex = (memoryRangeBase / tableManSize);
            UINTN endTableIndex = ((memoryRangeLast - 1) / tableManSize);

            UINTN numRangeTables = endTableIndex - startTableIndex;
            if (highestTableIndices[iLevel] < startTableIndex)
                numRangeTables += 1;

            highestTableIndices[iLevel] = endTableIndex;
            numTablesPerLevel[iLevel] += numRangeTables;
        }

        highestResolvedAddress = memoryRangeLast;
    }


    // Add 1 as this is the only one PML4 table
    UINTN numAllTables = numTablesPerLevel[0] + numTablesPerLevel[1] + numTablesPerLevel[2] + 1;

    UINTN allocationSize = numAllTables * DEXPROSBOOT_SINGLE_PAGE_MAP_TABLE_SIZE;


    (void)memoryDescriptorVersion;

    return allocationSize;
}

UINTN DexprOSBoot_CalculatePageMap5SizeForLoader(const void* pUefiMemoryMap,
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
        1, 1, 1, 1
    };



    EFI_PHYSICAL_ADDRESS highestResolvedAddress = 0;

    EFI_PHYSICAL_ADDRESS highestTableIndices[4] = {
        0, 0, 0, 0
    };

    for (;;)
    {
        EFI_PHYSICAL_ADDRESS memoryRangeBase = 0;
        EFI_PHYSICAL_ADDRESS memoryRangeLast = 0;

        // Find the next lowest continuous memory region to process
        const unsigned char* pUefiMemoryMapBytes = (const unsigned char*)pUefiMemoryMap;
        for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
        {
            const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);

            // First, we want to map only the memory ranges that are crucial
            // for the bootloader to function. The rest of them will be mapped later.
            if (!ShouldIncludeMemoryTypeInPageMap(pMemoryDesc->Type))
                continue;


            EFI_PHYSICAL_ADDRESS descStart = pMemoryDesc->PhysicalStart;
            EFI_PHYSICAL_ADDRESS mapMin = descStart > highestResolvedAddress ? descStart : highestResolvedAddress;
            EFI_PHYSICAL_ADDRESS mapMax = descStart + pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE;

            if (mapMax > highestResolvedAddress)
            {
                if (memoryRangeLast == 0)
                {
                    memoryRangeBase = mapMin;
                    memoryRangeLast = mapMax;
                }
                else if (mapMin < memoryRangeBase)
                {
                    memoryRangeBase = mapMin;
                    memoryRangeLast = mapMax;
                }
                else if (mapMin == memoryRangeBase && mapMax > memoryRangeLast)
                {
                    memoryRangeLast = mapMax;
                }
            }
        }
        if (framebufferBase + framebufferSize > highestResolvedAddress)
        {
            EFI_PHYSICAL_ADDRESS mapMin = framebufferBase > highestResolvedAddress ? framebufferBase : highestResolvedAddress;
            EFI_PHYSICAL_ADDRESS mapMax = framebufferBase + framebufferSize;

            if (memoryRangeLast == 0)
            {
                memoryRangeBase = mapMin;
                memoryRangeLast = mapMax;
            }
            else if (mapMin < memoryRangeBase)
            {
                memoryRangeBase = mapMin;
                memoryRangeLast = mapMax;
            }
            else if (mapMin == memoryRangeBase && mapMax > memoryRangeLast)
            {
                memoryRangeLast = mapMax;
            }
        }


        // No memory ranges left
        if (memoryRangeLast == 0)
            break;

        // Count additional table levels
        for (int iLevel = 0; iLevel < 4; ++iLevel)
        {
            UINTN tableManSize = managedMemorySizePerTableLevel[iLevel];

            UINTN startTableIndex = (memoryRangeBase / tableManSize);
            UINTN endTableIndex = ((memoryRangeLast - 1) / tableManSize);

            UINTN numRangeTables = endTableIndex - startTableIndex;
            if (highestTableIndices[iLevel] < startTableIndex)
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


    (void)memoryDescriptorVersion;

    return allocationSize;
}


static bool PrepareLevel1Table(EFI_PHYSICAL_ADDRESS startAddress,
                               EFI_PHYSICAL_ADDRESS endAddress,
                               uint64_t* pLevel2Table,
                               uint64_t** ppNextAvailableTableSpace,
                               uint64_t* pPageMapBufferEnd)
{
    uint64_t level2Offset = ((startAddress >> 21) & 0x1FF);

    uint64_t* pLevel2Entry = pLevel2Table + level2Offset;

    uint64_t* pLevel1Table = NULL;

    if ((*pLevel2Entry & DEXPROSBOOT_PAGE_PRESENT_BIT) == 0)
    {
        if (*ppNextAvailableTableSpace + 512 > pPageMapBufferEnd)
            return false;
        
        pLevel1Table = *ppNextAvailableTableSpace;
        *ppNextAvailableTableSpace += 512;

        for (int i = 0; i < 512; ++i)
            pLevel1Table[i] = 0;

        *pLevel2Entry |= ((uint64_t)pLevel1Table & 0xFFFFFFFFFFFFF000);
        *pLevel2Entry |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);
    }
    else
    {
        pLevel1Table = (uint64_t*)(*pLevel2Entry & 0xFFFFFFFFFFFFF000);
    }

    
    EFI_PHYSICAL_ADDRESS entryAddress = (startAddress & 0xFFFFFFFFFFFFF000);

    while (entryAddress < endAddress)
    {
        uint64_t entryIndex = ((entryAddress >> 12) & 0x1FF);
        pLevel1Table[entryIndex] |= entryAddress;
        pLevel1Table[entryIndex] |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);

        entryAddress += DEXPROSBOOT_PAGE_SIZE;
    }

    return true;
}

static bool PrepareLevel2Table(EFI_PHYSICAL_ADDRESS startAddress,
                               EFI_PHYSICAL_ADDRESS endAddress,
                               uint64_t* pLevel3Table,
                               uint64_t** ppNextAvailableTableSpace,
                               uint64_t* pPageMapBufferEnd)
{
    uint64_t level3Offset = ((startAddress >> 30) & 0x1FF);

    uint64_t* pLevel3Entry = pLevel3Table + level3Offset;

    uint64_t* pLevel2Table = NULL;

    if ((*pLevel3Entry & DEXPROSBOOT_PAGE_PRESENT_BIT) == 0)
    {
        if (*ppNextAvailableTableSpace + 512 > pPageMapBufferEnd)
            return false;
        
        pLevel2Table = *ppNextAvailableTableSpace;
        *ppNextAvailableTableSpace += 512;

        for (int i = 0; i < 512; ++i)
            pLevel2Table[i] = 0;

        *pLevel3Entry |= ((uint64_t)pLevel2Table & 0xFFFFFFFFFFFFF000);
        *pLevel3Entry |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);
    }
    else
    {
        pLevel2Table = (uint64_t*)(*pLevel3Entry & 0xFFFFFFFFFFFFF000);
    }


    const UINTN managedMemorySize = DEXPROSBOOT_LEVEL1_PAGE_TABLE_MANAGED_BYTES;
    EFI_PHYSICAL_ADDRESS alignedStartAddress = (startAddress / managedMemorySize) * managedMemorySize;

    EFI_PHYSICAL_ADDRESS subtableStartAddress = startAddress;
    EFI_PHYSICAL_ADDRESS subtableEndAddress = alignedStartAddress + managedMemorySize;
    subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;

    while (subtableStartAddress < endAddress)
    {
        if (!PrepareLevel1Table(subtableStartAddress,
                                subtableEndAddress,
                                pLevel2Table,
                                ppNextAvailableTableSpace,
                                pPageMapBufferEnd))
            return false;

        subtableStartAddress = (subtableStartAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = (subtableEndAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;
    }

    return true;
}

static bool PrepareLevel3Table(EFI_PHYSICAL_ADDRESS startAddress,
                               EFI_PHYSICAL_ADDRESS endAddress,
                               uint64_t* pPML4Table,
                               uint64_t** ppNextAvailableTableSpace,
                               uint64_t* pPageMapBufferEnd)
{
    uint64_t level4Offset = ((startAddress >> 39) & 0x1FF);

    uint64_t* pLevel4Entry = pPML4Table + level4Offset;

    uint64_t* pLevel3Table = NULL;

    if ((*pLevel4Entry & DEXPROSBOOT_PAGE_PRESENT_BIT) == 0)
    {
        if (*ppNextAvailableTableSpace + 512 > pPageMapBufferEnd)
            return false;
        
        pLevel3Table = *ppNextAvailableTableSpace;
        *ppNextAvailableTableSpace += 512;

        for (int i = 0; i < 512; ++i)
            pLevel3Table[i] = 0;

        *pLevel4Entry |= ((uint64_t)pLevel3Table & 0xFFFFFFFFFFFFF000);
        *pLevel4Entry |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);
    }
    else
    {
        pLevel3Table = (uint64_t*)(*pLevel4Entry & 0xFFFFFFFFFFFFF000);
    }


    const UINTN managedMemorySize = DEXPROSBOOT_LEVEL2_PAGE_TABLE_MANAGED_BYTES;
    EFI_PHYSICAL_ADDRESS alignedStartAddress = (startAddress / managedMemorySize) * managedMemorySize;

    EFI_PHYSICAL_ADDRESS subtableStartAddress = startAddress;
    EFI_PHYSICAL_ADDRESS subtableEndAddress = alignedStartAddress + managedMemorySize;
    subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;

    while (subtableStartAddress < endAddress)
    {
        if (!PrepareLevel2Table(subtableStartAddress,
                                subtableEndAddress,
                                pLevel3Table,
                                ppNextAvailableTableSpace,
                                pPageMapBufferEnd))
            return false;

        subtableStartAddress = (subtableStartAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = (subtableEndAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;
    }

    return true;
}

static bool PrepareLevel4Table(EFI_PHYSICAL_ADDRESS startAddress,
                               EFI_PHYSICAL_ADDRESS endAddress,
                               uint64_t* pPML5Table,
                               uint64_t** ppNextAvailableTableSpace,
                               uint64_t* pPageMapBufferEnd)
{
    uint64_t level5Offset = ((startAddress >> 48) & 0x1FF);

    uint64_t* pLevel5Entry = pPML5Table + level5Offset;

    uint64_t* pLevel4Table = NULL;

    if ((*pLevel5Entry & DEXPROSBOOT_PAGE_PRESENT_BIT) == 0)
    {
        if (*ppNextAvailableTableSpace + 512 > pPageMapBufferEnd)
            return false;
        
        pLevel4Table = *ppNextAvailableTableSpace;
        *ppNextAvailableTableSpace += 512;

        for (int i = 0; i < 512; ++i)
            pLevel4Table[i] = 0;

        *pLevel5Entry |= ((uint64_t)pLevel4Table & 0xFFFFFFFFFFFFF000);
        *pLevel5Entry |= (DEXPROSBOOT_PAGE_PRESENT_BIT | DEXPROSBOOT_PAGE_READWRITE_BIT);
    }
    else
    {
        pLevel4Table = (uint64_t*)(*pLevel5Entry & 0xFFFFFFFFFFFFF000);
    }


    const UINTN managedMemorySize = DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES;
    EFI_PHYSICAL_ADDRESS alignedStartAddress = (startAddress / managedMemorySize) * managedMemorySize;

    EFI_PHYSICAL_ADDRESS subtableStartAddress = startAddress;
    EFI_PHYSICAL_ADDRESS subtableEndAddress = alignedStartAddress + managedMemorySize;
    subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;

    while (subtableStartAddress < endAddress)
    {
        if (!PrepareLevel3Table(subtableStartAddress,
                                subtableEndAddress,
                                pLevel4Table,
                                ppNextAvailableTableSpace,
                                pPageMapBufferEnd))
            return false;

        subtableStartAddress = (subtableStartAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = (subtableEndAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;
    }

    return true;
}


static bool MapMemoryRangeInLevel4Table(EFI_PHYSICAL_ADDRESS startAddress,
                                        EFI_PHYSICAL_ADDRESS endAddress,
                                        uint64_t* pPML4Table,
                                        uint64_t** ppNextAvailableTableSpace,
                                        uint64_t* pPageMapBufferEnd)
{
    const UINTN managedMemorySize = DEXPROSBOOT_LEVEL3_PAGE_TABLE_MANAGED_BYTES;
    EFI_PHYSICAL_ADDRESS alignedStartAddress = (startAddress / managedMemorySize) * managedMemorySize;

    EFI_PHYSICAL_ADDRESS subtableStartAddress = startAddress;
    EFI_PHYSICAL_ADDRESS subtableEndAddress = alignedStartAddress + managedMemorySize;
    subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;

    while (subtableStartAddress < endAddress)
    {
        if (!PrepareLevel3Table(subtableStartAddress,
                                subtableEndAddress,
                                pPML4Table,
                                ppNextAvailableTableSpace,
                                pPageMapBufferEnd))
            return false;

        subtableStartAddress = (subtableStartAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = (subtableEndAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;
    }

    return true;
}


static bool MapMemoryRangeInLevel5Table(EFI_PHYSICAL_ADDRESS startAddress,
                                        EFI_PHYSICAL_ADDRESS endAddress,
                                        uint64_t* pPML5Table,
                                        uint64_t** ppNextAvailableTableSpace,
                                        uint64_t* pPageMapBufferEnd)
{
    const UINTN managedMemorySize = DEXPROSBOOT_LEVEL4_PAGE_TABLE_MANAGED_BYTES;
    EFI_PHYSICAL_ADDRESS alignedStartAddress = (startAddress / managedMemorySize) * managedMemorySize;

    EFI_PHYSICAL_ADDRESS subtableStartAddress = startAddress;
    EFI_PHYSICAL_ADDRESS subtableEndAddress = alignedStartAddress + managedMemorySize;
    subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;

    while (subtableStartAddress < endAddress)
    {
        if (!PrepareLevel4Table(subtableStartAddress,
                                subtableEndAddress,
                                pPML5Table,
                                ppNextAvailableTableSpace,
                                pPageMapBufferEnd))
            return false;

        subtableStartAddress = (subtableStartAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = (subtableEndAddress / managedMemorySize + 1) * managedMemorySize;
        subtableEndAddress = subtableEndAddress < endAddress ? subtableEndAddress : endAddress;
    }

    return true;
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
    if (((size_t)pPageMapBuffer % DEXPROSBOOT_PAGE_SIZE) != 0)
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
        if (!ShouldIncludeMemoryTypeInPageMap(pMemoryDesc->Type))
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


void* DexprOSBoot_SetupInitialPageMap5(const void* pUefiMemoryMap,
                                       UINTN memoryMapSize,
                                       UINTN memoryDescriptorSize,
                                       UINTN memoryDescriptorVersion,
                                       EFI_PHYSICAL_ADDRESS framebufferBase,
                                       UINTN framebufferSize,
                                       void* pPageMapBuffer,
                                       UINTN pageMapBufferSize)
{
    if (((size_t)pPageMapBuffer % DEXPROSBOOT_PAGE_SIZE) != 0)
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
        if (!ShouldIncludeMemoryTypeInPageMap(pMemoryDesc->Type))
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


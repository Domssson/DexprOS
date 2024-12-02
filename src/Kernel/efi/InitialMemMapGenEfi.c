#include "DexprOS/Kernel/efi/InitialMemMapGenEfi.h"

#include "DexprOS/Kernel/kstdlib/string.h"


static DexprOS_PhysicalMemoryType ToDexprOSMemType(EFI_MEMORY_TYPE efiType)
{
    switch (efiType)
    {
    case EfiUnusableMemory:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE;

    case EfiReservedMemoryType:
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
    case EfiPalCode:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED;

    case EfiUnacceptedMemoryType:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED;

    case EfiConventionalMemory:
    case EfiLoaderCode:
    case EfiLoaderData:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE;

    case EfiPersistentMemory:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE_PERSISTENT;

    case EfiACPIReclaimMemory:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM;

    case EfiACPIMemoryNVS:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS;

    case EfiBootServicesCode:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE;
    case EfiBootServicesData:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA;

    case EfiRuntimeServicesCode:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE;

    case EfiRuntimeServicesData:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA;
    
    default:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED;
    }
}

static DexprOS_InitialMemMapMappedUsage ToDexprOSInitialMemMapUsage(EFI_MEMORY_TYPE efiType)
{
    switch (efiType)
    {
    

    case EfiConventionalMemory:
    case EfiPersistentMemory:
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
        return DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_MAPPED;

    case EfiLoaderCode:
        return DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_CODE;
    case EfiLoaderData:
        return DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA;
    
    case EfiUnusableMemory:
    case EfiReservedMemoryType:
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
    case EfiPalCode:
    case EfiUnacceptedMemoryType:
    case EfiACPIReclaimMemory:
    case EfiACPIMemoryNVS:
    default:
        return DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE;
    }
}

static uint64_t ToDexprOSAttributeFlags(UINT64 efiAttributes)
{
    uint64_t result = 0;

    if ((efiAttributes & EFI_MEMORY_UC) == EFI_MEMORY_UC)
        result |= DEXPROS_PHYSICAL_MEMORY_UC_BIT;

    if ((efiAttributes & EFI_MEMORY_WC) == EFI_MEMORY_WC)
        result |= DEXPROS_PHYSICAL_MEMORY_WRITE_COMBINE_BIT;

    if ((efiAttributes & EFI_MEMORY_WT) == EFI_MEMORY_WT)
        result |= DEXPROS_PHYSICAL_MEMORY_WRITE_THROUGH_BIT;

    if ((efiAttributes & EFI_MEMORY_WB) == EFI_MEMORY_WB)
        result |= DEXPROS_PHYSICAL_MEMORY_WRITE_BACK_BIT;

    if ((efiAttributes & EFI_MEMORY_UCE) == EFI_MEMORY_UCE)
        result |= DEXPROS_PHYSICAL_MEMORY_UCE_BIT;

    if ((efiAttributes & EFI_MEMORY_WP) == EFI_MEMORY_WP)
        result |= DEXPROS_PHYSICAL_MEMORY_WRITE_PROTECTED_BIT;

    if ((efiAttributes & EFI_MEMORY_RP) == EFI_MEMORY_RP)
        result |= DEXPROS_PHYSICAL_MEMORY_READ_PROTECTED_BIT;

    if ((efiAttributes & EFI_MEMORY_XP) == EFI_MEMORY_XP)
        result |= DEXPROS_PHYSICAL_MEMORY_EXECUTE_PROTECTED_BIT;

    if ((efiAttributes & EFI_MEMORY_RO) == EFI_MEMORY_RO)
        result |= DEXPROS_PHYSICAL_MEMORY_READ_ONLY_BIT;

    if ((efiAttributes & EFI_MEMORY_NV) == EFI_MEMORY_NV)
        result |= DEXPROS_PHYSICAL_MEMORY_NON_VOLATILE_BIT;

    if ((efiAttributes & EFI_MEMORY_RUNTIME) == EFI_MEMORY_RUNTIME)
        result |= DEXPROS_PHYSICAL_MEMORY_EFI_RUNTIME_BIT;

    if ((efiAttributes & EFI_MEMORY_MORE_RELIABLE) == EFI_MEMORY_MORE_RELIABLE)
        result |= DEXPROS_PHYSICAL_MEMORY_MORE_RELIABLE_BIT;

    if ((efiAttributes & EFI_MEMORY_SP) == EFI_MEMORY_SP)
        result |= DEXPROS_PHYSICAL_MEMORY_SPECIFIC_PURPOSE_BIT;

    if ((efiAttributes & EFI_MEMORY_CPU_CRYPTO) == EFI_MEMORY_CPU_CRYPTO)
        result |= DEXPROS_PHYSICAL_MEMORY_CPU_CRYPTO_BIT;

    if ((efiAttributes & EFI_MEMORY_ISA_VALID) == EFI_MEMORY_ISA_VALID)
    {
        result |= DEXPROS_PHYSICAL_MEMORY_ISA_VALID_BIT;

        // EFI_MEMORY_ISA_MASK and DEXPROS_PHYSICAL_MEMORY_ISA_MASK_BITS are
        // defined to the same value
        result |= (efiAttributes & EFI_MEMORY_ISA_MASK);
    }

    return result;
}


static bool GetNextEfiMapEntry(const void* pUefiMemoryMap,
                               UINTN memoryMapSize,
                               UINTN memoryDescriptorSize,
                               UINTN memoryDescriptorVersion,
                               DexprOS_PhysicalMemoryAddress minAddress,
                               DexprOS_InitialMemMapEntry* pOutMemRange)
{
    const unsigned char* pUefiMemMapBytes = (const unsigned char*)pUefiMemoryMap;

    bool nextDescFound = false;


    pOutMemRange->memoryType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
    pOutMemRange->usage = DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE;
    pOutMemRange->flags = 0;
    pOutMemRange->physicalAddress = 0;
    pOutMemRange->virtualAddress = 0;
    pOutMemRange->numPhysicalPages = 0;
    

    for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
    {
        const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemMapBytes + memOffset);

        if (pMemoryDesc->PhysicalStart >= minAddress &&
            (pMemoryDesc->PhysicalStart < pOutMemRange->physicalAddress || !nextDescFound))
        {
            pOutMemRange->physicalAddress = pMemoryDesc->PhysicalStart;
            /* We still use identity paging at this point */
            pOutMemRange->virtualAddress = pMemoryDesc->PhysicalStart;
            pOutMemRange->numPhysicalPages = pMemoryDesc->NumberOfPages * EFI_PAGE_SIZE / DEXPROS_PHYSICAL_PAGE_SIZE;
            pOutMemRange->memoryType = ToDexprOSMemType(pMemoryDesc->Type);
            pOutMemRange->usage = ToDexprOSInitialMemMapUsage(pMemoryDesc->Type);
            pOutMemRange->flags = ToDexprOSAttributeFlags(pMemoryDesc->Attribute);
            nextDescFound = true;
        }
    }


    (void)memoryDescriptorVersion;

    return nextDescFound;
}


typedef void (*DexprOS_InitMapCreateIterFunc)(DexprOS_InitialMemMapEntry memEntry,
                                              void*                      pUserData);


static void IterEfiMemMapAsInitMap(const void* pUefiMemoryMap,
                                   UINTN memoryMapSize,
                                   UINTN memoryDescriptorSize,
                                   UINTN memoryDescriptorVersion,
                                   const DexprOS_InitMapCreateIterFunc pFunc,
                                   void* pUserData)
{
    DexprOS_InitialMemMapEntry combinedRange;
    combinedRange.memoryType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
    combinedRange.usage = DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE;
    combinedRange.flags = 0;
    combinedRange.physicalAddress = 0;
    combinedRange.virtualAddress = 0;
    combinedRange.numPhysicalPages = 0;


    bool anyRangeWritten = false;

    for (;;)
    {
        DexprOS_PhysicalMemoryAddress rangeEnd = combinedRange.physicalAddress +
                                                 combinedRange.numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;
        DexprOS_VirtualMemoryAddress virtRangeEnd = combinedRange.virtualAddress +
                                                    combinedRange.numPhysicalPages * DEXPROS_PHYSICAL_PAGE_SIZE;

        DexprOS_InitialMemMapEntry nextMemRegion;

        if (!GetNextEfiMapEntry(pUefiMemoryMap,
                                memoryMapSize,
                                memoryDescriptorSize,
                                memoryDescriptorVersion,
                                rangeEnd,
                                &nextMemRegion))
            break;


        // Combine ranges whenever possible
        if (nextMemRegion.physicalAddress == rangeEnd &&
            nextMemRegion.virtualAddress == virtRangeEnd &&
            nextMemRegion.memoryType == combinedRange.memoryType &&
            nextMemRegion.usage == combinedRange.usage &&
            nextMemRegion.flags == combinedRange.flags &&
            anyRangeWritten)
        {
            combinedRange.numPhysicalPages += nextMemRegion.numPhysicalPages;
        }
        else
        {
            // Submit previous range if it exists
            if (anyRangeWritten)
            {
                pFunc(combinedRange, pUserData);
            }

            combinedRange.physicalAddress = nextMemRegion.physicalAddress;
            combinedRange.virtualAddress = nextMemRegion.virtualAddress;
            combinedRange.numPhysicalPages = nextMemRegion.numPhysicalPages;
            combinedRange.memoryType = nextMemRegion.memoryType;
            combinedRange.usage = nextMemRegion.usage;
            combinedRange.flags = nextMemRegion.flags;
            anyRangeWritten = true;
        }
    }


    // Submit previous range if it exists
    if (anyRangeWritten)
    {
        pFunc(combinedRange, pUserData);
    }
}


typedef struct DexprOS_InitMemMapAllocStruct
{
    size_t numRegions;

    DexprOS_PhysicalMemoryAddress largestUsableRegionAddress;
    DexprOS_VirtualMemoryAddress virtualAddress;
    size_t numLargestUsableRegionPages;
} InitMemMapAllocStruct;

static void InitMemMapAllocCallback(DexprOS_InitialMemMapEntry memEntry,
                                    void* pUserData)
{
    InitMemMapAllocStruct* pAllocStruct = (InitMemMapAllocStruct*)pUserData;
    pAllocStruct->numRegions += 1;

    if (memEntry.memoryType == DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE &&
        memEntry.usage == DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_MAPPED &&
        memEntry.numPhysicalPages > pAllocStruct->numLargestUsableRegionPages)
    {
        pAllocStruct->largestUsableRegionAddress = memEntry.physicalAddress;
        pAllocStruct->virtualAddress = memEntry.virtualAddress;
        pAllocStruct->numLargestUsableRegionPages = memEntry.numPhysicalPages;
    }
}

static bool AllocInitMemMapMemory(const void* pUefiMemoryMap,
                                  UINTN memoryMapSize,
                                  UINTN memoryDescriptorSize,
                                  UINTN memoryDescriptorVersion,
                                  DexprOS_PhysicalMemoryAddress* pOutPhysAddress,
                                  DexprOS_VirtualMemoryAddress* pOutVirtAddress,
                                  size_t* pOutSize)
{
    InitMemMapAllocStruct allocStruct;
    allocStruct.numRegions = 0;
    allocStruct.largestUsableRegionAddress = 0;
    allocStruct.virtualAddress = 0;
    allocStruct.numLargestUsableRegionPages = 0;

    IterEfiMemMapAsInitMap(pUefiMemoryMap,
                           memoryMapSize,
                           memoryDescriptorSize,
                           memoryDescriptorVersion,
                           InitMemMapAllocCallback,
                           &allocStruct);

    // Add 1 because there will be one additional entry telling where the map itself is
    allocStruct.numRegions += 1;


    size_t requiredSize = allocStruct.numRegions * sizeof(DexprOS_InitialMemMapEntry);
    size_t requiredPages = (requiredSize + DEXPROS_PHYSICAL_PAGE_SIZE - 1) / DEXPROS_PHYSICAL_PAGE_SIZE;

    // Use the beginning of the largest region as the buffer for the map
    if (allocStruct.numLargestUsableRegionPages >= requiredPages)
    {
        *pOutPhysAddress = allocStruct.largestUsableRegionAddress;
        *pOutVirtAddress = allocStruct.virtualAddress;
        *pOutSize = requiredSize;
        return true;
    }

    return false;
}



typedef struct DexprOS_InitMemMapCreationStruct
{
    DexprOS_InitialMemMap* pMemMap;
    size_t availableSize;

    DexprOS_PhysicalMemoryAddress initMapPhysAddress;
    DexprOS_VirtualMemoryAddress initMapVirtAddress;
    size_t numInitMapPages;

    bool success;
} InitMemMapCreationStruct;


static void FillInitMemMapEntry(DexprOS_InitialMemMapEntry memEntry,
                                void* pUserData)
{
    InitMemMapCreationStruct* pCreateInfo = (InitMemMapCreationStruct*)pUserData;

    // Here we benefit the fact that the allocation buffer will always be at
    // the beginning of a usable memory region
    if (memEntry.physicalAddress == pCreateInfo->initMapPhysAddress)
    {
        if (pCreateInfo->pMemMap->numEntries * sizeof(DexprOS_InitialMemMapEntry) >= pCreateInfo->availableSize)
        {
            pCreateInfo->success = false;
            return;
        }
        DexprOS_InitialMemMapEntry* pEntry = &pCreateInfo->pMemMap->pEntries[pCreateInfo->pMemMap->numEntries];
        pCreateInfo->pMemMap->numEntries += 1;
        
        *pEntry = memEntry;
        pEntry->usage = DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_INITIAL_MEMMAP;
        pEntry->numPhysicalPages = pCreateInfo->numInitMapPages;

        if (memEntry.numPhysicalPages == pCreateInfo->numInitMapPages)
            return;


        // If there are some pages left, create another entry describing
        // the remaining pages
        if (pCreateInfo->pMemMap->numEntries * sizeof(DexprOS_InitialMemMapEntry) >= pCreateInfo->availableSize)
        {
            pCreateInfo->success = false;
            return;
        }
        pEntry += 1;
        pCreateInfo->pMemMap->numEntries += 1;

        *pEntry = memEntry;
        pEntry->physicalAddress += pCreateInfo->numInitMapPages * DEXPROS_PHYSICAL_PAGE_SIZE;
        pEntry->virtualAddress += pCreateInfo->numInitMapPages * DEXPROS_PHYSICAL_PAGE_SIZE;
        pEntry->numPhysicalPages -= pCreateInfo->numInitMapPages;
    }
    else
    {
        if (pCreateInfo->pMemMap->numEntries * sizeof(DexprOS_InitialMemMapEntry) >= pCreateInfo->availableSize)
        {
            pCreateInfo->success = false;
            return;
        }
        pCreateInfo->pMemMap->pEntries[pCreateInfo->pMemMap->numEntries] = memEntry;
        pCreateInfo->pMemMap->numEntries += 1;
    }
}

static bool CreateInitMemMap(const void* pUefiMemoryMap,
                             UINTN memoryMapSize,
                             UINTN memoryDescriptorSize,
                             UINTN memoryDescriptorVersion,
                             DexprOS_PhysicalMemoryAddress physAddress,
                             DexprOS_VirtualMemoryAddress virtAddress,
                             size_t availableSize,
                             DexprOS_InitialMemMap* pOutMemMap)
{
    InitMemMapCreationStruct initMapStruct;

    pOutMemMap->pEntries = (DexprOS_InitialMemMapEntry*)virtAddress;
    pOutMemMap->numEntries = 0;

    initMapStruct.pMemMap = pOutMemMap;
    initMapStruct.availableSize = availableSize;
    initMapStruct.initMapPhysAddress = physAddress;
    initMapStruct.initMapVirtAddress = virtAddress;
    initMapStruct.numInitMapPages = (availableSize + DEXPROS_PHYSICAL_PAGE_SIZE - 1) / DEXPROS_PHYSICAL_PAGE_SIZE;
    initMapStruct.success = true;

    IterEfiMemMapAsInitMap(pUefiMemoryMap,
                           memoryMapSize,
                           memoryDescriptorSize,
                           memoryDescriptorVersion,
                           FillInitMemMapEntry,
                           &initMapStruct);

    return initMapStruct.success;
}


bool DexprOS_CreateInitialMemMapFromEfi(const void* pUefiMemoryMap,
                                        UINTN memoryMapSize,
                                        UINTN memoryDescriptorSize,
                                        UINTN memoryDescriptorVersion,
                                        DexprOS_InitialMemMap* pOutMemMap)
{
    DexprOS_PhysicalMemoryAddress initMapPhysAddress = 0;
    DexprOS_VirtualMemoryAddress initMapVirtAddress = 0;
    size_t initMapSize = 0;

    if (!AllocInitMemMapMemory(pUefiMemoryMap,
                               memoryMapSize,
                               memoryDescriptorSize,
                               memoryDescriptorVersion,
                               &initMapPhysAddress,
                               &initMapVirtAddress,
                               &initMapSize))
        return false;

    if (!CreateInitMemMap(pUefiMemoryMap,
                          memoryMapSize,
                          memoryDescriptorSize,
                          memoryDescriptorVersion,
                          initMapPhysAddress,
                          initMapVirtAddress,
                          initMapSize,
                          pOutMemMap))
        return false;

    return true;
}


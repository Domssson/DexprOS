#include "DexprOS/Kernel/efi/PhysicalMemMapEfi.h"


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
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiLoaderCode:
    case EfiLoaderData:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE;

    case EfiPersistentMemory:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE_PERSISTENT;

    case EfiACPIReclaimMemory:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM;

    case EfiACPIMemoryNVS:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS;

    case EfiRuntimeServicesCode:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE;

    case EfiRuntimeServicesData:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA;
    
    default:
        return DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED;
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


size_t DexprOS_GetPhysicalMemMapSizeFromEfi(const void* pUefiMemoryMap,
                                            UINTN memoryMapSize,
                                            UINTN memoryDescriptorSize,
                                            UINTN memoryDescriptorVersion)
{
    size_t numEntries = 0;


    EFI_PHYSICAL_ADDRESS lastRangeEnd = 0;
    DexprOS_PhysicalMemoryType lastRangeType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
    UINT64 lastRangeAttributes = 0;

    const unsigned char* pUefiMemMapBytes = (const unsigned char*)pUefiMemoryMap;

    for (;;)
    {
        EFI_PHYSICAL_ADDRESS lowestAddress = 0;
        UINTN numPages = 0;
        DexprOS_PhysicalMemoryType memType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
        UINT64 memAttributes = 0;

        bool nextDescFound = false;

        for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
        {
            const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemMapBytes + memOffset);

            if (pMemoryDesc->PhysicalStart >= lastRangeEnd &&
                (pMemoryDesc->PhysicalStart < lowestAddress || !nextDescFound))
            {
                lowestAddress = pMemoryDesc->PhysicalStart;
                numPages = pMemoryDesc->NumberOfPages;
                memType = ToDexprOSMemType(pMemoryDesc->Type);
                memAttributes = pMemoryDesc->Attribute;
                nextDescFound = true;
            }
        }

        if (!nextDescFound)
            break;

        // Combine ranges wherever possible
        if (lastRangeEnd != lowestAddress ||
            memType != lastRangeType ||
            memAttributes != lastRangeAttributes ||
            numEntries == 0)
        {
            numEntries += 1;
        }

        lastRangeEnd = lowestAddress + numPages * EFI_PAGE_SIZE;
        lastRangeType = memType;
        lastRangeAttributes = memAttributes;
    }



    size_t totalSize = numEntries * sizeof(DexprOS_PhysicalMemoryRange);

    (void)memoryDescriptorVersion;

    return totalSize;
}


bool DexprOS_CreatePhysicalMemMapFromEfi(DexprOS_PhysicalMemMap* pResult,
                                         const void* pUefiMemoryMap,
                                         UINTN memoryMapSize,
                                         UINTN memoryDescriptorSize,
                                         UINTN memoryDescriptorVersion,
                                         void* pBuffer,
                                         size_t bufferSize)
{
    DexprOS_PhysicalMemoryRange* pEntries = (DexprOS_PhysicalMemoryRange*)pBuffer;


    size_t index = 0;
    bool firstRangeWrite = true;


    EFI_PHYSICAL_ADDRESS lastRangeEnd = 0;
    DexprOS_PhysicalMemoryType lastRangeType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
    UINT64 lastRangeAttributes = 0;

    const unsigned char* pUefiMemMapBytes = (const unsigned char*)pUefiMemoryMap;

    for (;;)
    {
        EFI_PHYSICAL_ADDRESS lowestAddress = 0;
        UINTN numPages = 0;
        DexprOS_PhysicalMemoryType memType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
        UINT64 memAttributes = 0;

        bool nextDescFound = false;

        for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
        {
            const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemMapBytes + memOffset);

            if (pMemoryDesc->PhysicalStart >= lastRangeEnd &&
                (pMemoryDesc->PhysicalStart < lowestAddress || !nextDescFound))
            {
                lowestAddress = pMemoryDesc->PhysicalStart;
                numPages = pMemoryDesc->NumberOfPages;
                memType = ToDexprOSMemType(pMemoryDesc->Type);
                memAttributes = pMemoryDesc->Attribute;
                nextDescFound = true;
            }
        }

        if (!nextDescFound)
            break;


        if (lastRangeEnd != lowestAddress ||
            lastRangeType != memType ||
            lastRangeAttributes != memAttributes)
        {
            if (!firstRangeWrite)
            {
                index += 1;
                if ((index + 1) * sizeof(DexprOS_PhysicalMemoryRange) > bufferSize)
                    return false;

                firstRangeWrite = true;
            }
        }
        

        if (firstRangeWrite)
        {
            pEntries[index].memoryType = memType;
            pEntries[index].physicalAddress = lowestAddress;
            pEntries[index].rangeSize = numPages * EFI_PAGE_SIZE;
            pEntries[index].flags = ToDexprOSAttributeFlags(memAttributes);
            firstRangeWrite = false;
        }
        else
        {
            pEntries[index].rangeSize += numPages * EFI_PAGE_SIZE;
        }


        lastRangeEnd = lowestAddress + numPages * EFI_PAGE_SIZE;
        lastRangeType = memType;
        lastRangeAttributes = memAttributes;
    }

    pResult->numEntries = index + 1;
    pResult->pEntries = pEntries;

    (void)memoryDescriptorVersion;

    return true;
}


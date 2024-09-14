#include "DexprOS/Kernel/efi/EfiMemMapIteration.h"

#include <stdbool.h>


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


void DexprOS_IterateEfiMemMap(const void* pUefiMemoryMap,
                              UINTN memoryMapSize,
                              UINTN memoryDescriptorSize,
                              UINTN memoryDescriptorVersion,
                              const DexprOS_EfiMemMapIterateFunc pFunc,
                              void* pUserData)
{
    DexprOS_PhysicalMemoryAddress rangeStart = 0;
    DexprOS_PhysicalMemorySize rangeSize = 0;
    DexprOS_PhysicalMemoryType rangeType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
    DexprOS_PhysicalMemoryFlags rangeFlags = 0;


    const unsigned char* pUefiMemMapBytes = (const unsigned char*)pUefiMemoryMap;

    bool anyRangeWritten = false;

    for (;;)
    {
        DexprOS_PhysicalMemoryAddress rangeEnd = rangeStart + rangeSize;

        EFI_PHYSICAL_ADDRESS lowestAddress = 0;
        UINTN numPages = 0;
        DexprOS_PhysicalMemoryType memType = DEXPROS_PHYSICAL_MEMORY_TYPE_Max;
        DexprOS_PhysicalMemoryFlags memFlags = 0;

        bool nextDescFound = false;

        for (UINTN memOffset = 0; memOffset < memoryMapSize; memOffset += memoryDescriptorSize)
        {
            const EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (const EFI_MEMORY_DESCRIPTOR*)(pUefiMemMapBytes + memOffset);

            if (pMemoryDesc->PhysicalStart >= rangeEnd &&
                (pMemoryDesc->PhysicalStart < lowestAddress || !nextDescFound))
            {
                lowestAddress = pMemoryDesc->PhysicalStart;
                numPages = pMemoryDesc->NumberOfPages;
                memType = ToDexprOSMemType(pMemoryDesc->Type);
                memFlags = ToDexprOSAttributeFlags(pMemoryDesc->Attribute);
                nextDescFound = true;
            }
        }

        if (!nextDescFound)
            break;


        // Combine ranges whenever possible
        if (lowestAddress == rangeEnd &&
            memType == rangeType &&
            memFlags == rangeFlags &&
            anyRangeWritten)
        {
            rangeSize += numPages * EFI_PAGE_SIZE;
        }
        else
        {
            // Submit previous range if it exists
            if (anyRangeWritten)
            {
                pFunc(rangeStart, rangeSize, rangeType, rangeFlags, pUserData);
            }

            rangeStart = lowestAddress;
            rangeSize = numPages * EFI_PAGE_SIZE;
            rangeType = memType;
            rangeFlags = memFlags;
            anyRangeWritten = true;
        }
    }


    // Submit previous range if it exists
    if (anyRangeWritten)
    {
        pFunc(rangeStart, rangeSize, rangeType, rangeFlags, pUserData);
    }


    (void)memoryDescriptorVersion;
}

#include "DexprOS/Kernel/efi/PhysicalMemTreeGenEfi.h"

#include "DexprOS/Kernel/efi/EfiMemMapIteration.h"
#include "DexprOS/Kernel/Memory/Paging.h"
#include "DexprOS/Kernel/Memory/MemoryDef.h"

#include <stdbool.h>


static bool ShouldCreateTreeForType(DexprOS_PhysicalMemoryType memType)
{
    switch (memType)
    {
    // Do not include boot services memory for allocation. There is a bug in
    // some UEFI implementations that causes the runtime services to access
    // boot services memory (https://wiki.osdev.org/Broken_UEFI_implementations).
    case DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE_PERSISTENT:
        return true;
    
    case DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA:
    case DEXPROS_PHYSICAL_MEMORY_TYPE_Max:
        return false;
    }
    return false;
}

typedef struct DexprOS_PhysMemTreeEfiSizeBuffer
{
    size_t numTreeEntries;
    size_t numTotalLevels;
    size_t totalBlocksSize;
} PhysMemTreeEfiSizeBuffer;

static void CalcTreeEntrySize(DexprOS_PhysicalMemoryAddress physicalAddress,
                              DexprOS_PhysicalMemorySize rangeSize,
                              DexprOS_PhysicalMemoryType memType,
                              DexprOS_PhysicalMemoryFlags memFlags,
                              void* pUserData)
{
    if (!ShouldCreateTreeForType(memType))
        return;

    PhysMemTreeEfiSizeBuffer* pSizeBuffer = (PhysMemTreeEfiSizeBuffer*)pUserData;
    pSizeBuffer->numTreeEntries += 1;

    const size_t numPages = rangeSize / DEXPROS_PAGE_SIZE;

    size_t numBlocks = numPages;

    for (;;)
    {
        pSizeBuffer->numTotalLevels += 1;
        
        if (numBlocks > sizeof(uint8_t*))
            pSizeBuffer->totalBlocksSize += numBlocks;

        if (numBlocks <= 1)
            return;

        numBlocks = (numBlocks + 1) / 2;
    }


    (void)physicalAddress;
    (void)memFlags;
}


size_t DexprOS_GetPhysicalMemTreeSizeFromEfi(const void* pUefiMemoryMap,
                                             UINTN memoryMapSize,
                                             UINTN memoryDescriptorSize,
                                             UINTN memoryDescriptorVersion)
{
    PhysMemTreeEfiSizeBuffer sizeBuffer;
    sizeBuffer.numTreeEntries = 0;
    sizeBuffer.numTotalLevels = 0;
    sizeBuffer.totalBlocksSize = 0;

    DexprOS_IterateEfiMemMap(pUefiMemoryMap,
                             memoryMapSize,
                             memoryDescriptorSize,
                             memoryDescriptorVersion,
                             CalcTreeEntrySize,
                             (void*)&sizeBuffer);

    size_t totalSize = 0;

    totalSize += sizeof(DexprOS_PhysicalMemTreeEntry) * sizeBuffer.numTreeEntries;
    // Align the size so the next array of structs can be safely stored
    totalSize = DEXPROS_ALIGN_FUNDAMENTAL(totalSize);

    totalSize += sizeof(DexprOS_PhysicalMemTreeLevel) * sizeBuffer.numTotalLevels;
    totalSize = DEXPROS_ALIGN_FUNDAMENTAL(totalSize);

    totalSize += sizeBuffer.totalBlocksSize;

    return totalSize;
}


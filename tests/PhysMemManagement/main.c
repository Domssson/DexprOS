#include "DexprOS/Kernel/efi/PhysicalMemStructsGenEfi.h"

#include <efi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    EFI_MEMORY_DESCRIPTOR efiMemoryMap[] = {
        {EfiBootServicesCode, 0, 0, 0, 256, 0xF},
        {EfiConventionalMemory, 0, 1048576, 1048576, 512, 0xF},
        {EfiLoaderCode, 0, 2148532224, 2148532224, 10, 0xF},
        {EfiLoaderData, 0, 2148573184, 2148573184, 20, 0xF},
        {EfiBootServicesData, 0, 2148655104, 2148655104, 32, 0xF},
        {EfiRuntimeServicesCode, 0, 2148786176, 2148786176, 8, 0xF},
        {EfiRuntimeServicesData, 0, 2148851712, 2148851712, 8, 0xF},
        {EfiLoaderCode, 0, 2148884480, 2148884480, 16, 0xF},
        {EfiLoaderData, 0, 2148950016, 2148950016, 33, 0xF},
        {EfiConventionalMemory, 0, 2149085184, 2149085184, 127, 0xF},
        {EfiACPIMemoryNVS, 0, 2149613568, 2149613568, 2, 0xF},
        {EfiACPIReclaimMemory, 0, 2149621760, 2149621760, 4, 0xF},
        {EfiConventionalMemory, 0, 2149638144, 2149638144, 256, 0xF},
        {EfiReservedMemoryType, 0, 2150686720, 2150686720, 8, 0xF},
        {EfiPersistentMemory, 0, 2150719488, 2150719488, 21, 0xF},
        {EfiUnusableMemory, 0, 2150805504, 2150805504, 16, 0xF},
        {EfiConventionalMemory, 0, 2150871040, 2150871040, 65536, 0xF}
    };

    void* pUefiMemoryMap = (void*)&efiMemoryMap[0];
    UINTN memoryMapSize = sizeof(efiMemoryMap);
    UINTN memoryDescriptorSize = sizeof(EFI_MEMORY_DESCRIPTOR);
    UINT32 memoryDescriptorVersion = EFI_MEMORY_DESCRIPTOR_VERSION;
    

    DexprOS_PhysMemStructsEfiSizeData sizeData = DexprOS_GetPhysicalMemStructsSizeDataFromEfi(pUefiMemoryMap,
                                                                                              memoryMapSize,
                                                                                              memoryDescriptorSize,
                                                                                              memoryDescriptorVersion);

    printf("Required buffer size: %lu\n", sizeData.bufferSize);


    void* pBuffer = malloc(sizeData.bufferSize);
    memset(pBuffer, 0, sizeData.bufferSize);


    DexprOS_PhysicalMemTree memTree;
    DexprOS_PhysicalMemMap memMap;
    if (!DexprOS_CreatePhysicalMemStructsFromEfi(&memTree,
                                                 &memMap,
                                                 pUefiMemoryMap,
                                                 memoryMapSize,
                                                 memoryDescriptorSize,
                                                 memoryDescriptorVersion,
                                                 pBuffer,
                                                 &sizeData))
    {
        free(pBuffer);
        return EXIT_FAILURE;
    }


    // Now just place debugger breakpoint here and inspect the memory ...
    printf("Structs creation success!\n");

    free(pBuffer);
}

#include "DexprOS/Boot/x86_64-efi/EndBootPhase.h"
#include "DexprOS/Kernel/Memory/MemoryDef.h"
#include "DexprOS/Kernel/efi/InitialMemMapGenEfi.h"
#include "DexprOS/Kernel/x86_64/CpuFeatures.h"
#include "DexprOS/Kernel/x86_64/MemoryProtectionCpuSetup.h"
#include "DexprOS/Kernel/x86_64/FloatingPointInit.h"
#include "DexprOS/Kernel/x86_64/PageMapSwitching.h"
#include "DexprOS/Kernel/x86_64/PagingSettings.h"
#include "DexprOS/Kernel/x86_64/StartupVirtMapCreator.h"
#include "DexprOS/Kernel/x86_64/KernelInit.h"
#include "DexprOS/Kernel/x86_64/Interrupts.h"

#include <efi.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


/*inline static int DexprOS_CompareUEFIGUID(const EFI_GUID* pGuid0,
                                          const EFI_GUID* pGuid1)
{
    return pGuid0->Data1 == pGuid1->Data1 &&
           pGuid0->Data2 == pGuid1->Data2 &&
           pGuid0->Data3 == pGuid1->Data3 &&
           pGuid0->Data4[0] == pGuid1->Data4[0] &&
           pGuid0->Data4[1] == pGuid1->Data4[1] &&
           pGuid0->Data4[2] == pGuid1->Data4[2] &&
           pGuid0->Data4[3] == pGuid1->Data4[3] &&
           pGuid0->Data4[4] == pGuid1->Data4[4] &&
           pGuid0->Data4[5] == pGuid1->Data4[5] &&
           pGuid0->Data4[6] == pGuid1->Data4[6] &&
           pGuid0->Data4[7] == pGuid1->Data4[7];
}*/


static EFI_STATUS RemapEfiAddressMap(EFI_SYSTEM_TABLE* pSystemTable,
                                     const DexprOS_EfiMemoryMap* pMemMap,
                                     DexprOS_VirtualMemoryAddress offset)
{
    unsigned char* pUefiMemoryMapBytes = (unsigned char*)pMemMap->pMemoryMapBuffer;
    for (UINTN memOffset = 0; memOffset < pMemMap->memoryMapSize; memOffset += pMemMap->memoryDescriptorSize)
    {
        EFI_MEMORY_DESCRIPTOR* pMemoryDesc = (EFI_MEMORY_DESCRIPTOR*)(pUefiMemoryMapBytes + memOffset);
        
        if (pMemoryDesc->Type == EfiRuntimeServicesCode ||
            pMemoryDesc->Type == EfiRuntimeServicesData ||
            (pMemoryDesc->Attribute & EFI_MEMORY_RUNTIME) == EFI_MEMORY_RUNTIME)
        {
            pMemoryDesc->VirtualStart = pMemoryDesc->PhysicalStart + offset;
        }
    }

    return pSystemTable->RuntimeServices->SetVirtualAddressMap(pMemMap->memoryMapSize,
                                                               pMemMap->memoryDescriptorSize,
                                                               pMemMap->memoryDescriptorVersion,
                                                               (EFI_MEMORY_DESCRIPTOR*)pMemMap->pMemoryMapBuffer);
}


EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* pSystemTable)
{
    EFI_STATUS status = 0;


    pSystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    if (EFI_ERROR(status))
        return status;


    // Setup graphics

    EFI_GUID gopProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* pGop = NULL;
    status = pSystemTable->BootServices->LocateProtocol(&gopProtocolGuid, NULL, (VOID**)&pGop);
    if (EFI_ERROR(status))
        return status;

    UINT32 currentVideoModeIndex = 0;
    if (pGop->Mode != NULL)
        currentVideoModeIndex = pGop->Mode->Mode;
    
    status = pGop->SetMode(pGop, currentVideoModeIndex);
    if (EFI_ERROR(status))
        return status;


    unsigned horizontalResolution = pGop->Mode->Info->HorizontalResolution;
    unsigned verticalResolution = pGop->Mode->Info->VerticalResolution;
    unsigned framebufferStride = pGop->Mode->Info->PixelsPerScanLine;

    DexprOS_VirtualMemoryAddress framebufferMem = pGop->Mode->FrameBufferBase;
    size_t framebufferSize = pGop->Mode->FrameBufferSize;
    EFI_GRAPHICS_PIXEL_FORMAT framebufferFormat = pGop->Mode->Info->PixelFormat;


    void* pFramebufferBackBuffer = NULL;
    status = pSystemTable->BootServices->AllocatePool(EfiLoaderData, pGop->Mode->FrameBufferSize, &pFramebufferBackBuffer);
    if (EFI_ERROR(status))
        return status;
    
    
    /*
    // Find the current FAT32 volume
    EFI_FILE_IO_INTERFACE* pVolumeIO = NULL;
    status = DexprOSBoot_FindBootloaderVolume(imageHandle,
                                              pSystemTable,
                                              &pVolumeIO);
    if (EFI_ERROR(status))
        return status;

    EFI_FILE_HANDLE volumeRoot = NULL;
    status = pVolumeIO->OpenVolume(pVolumeIO, &volumeRoot);
    if (EFI_ERROR(status))
        return status;


    CHAR16* testFileName = L"EFI\\BOOT\\TESTFILE.TXT";
    EFI_FILE_HANDLE testFileHandle = NULL;
    status = volumeRoot->Open(volumeRoot, &testFileHandle, testFileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status))
    {
        DexprOS_Puts("Could not load a test file.\n");
    }
    else
    {
        UINTN readBufferSize = 512;
        char readBuffer[513] = {0};

        status = testFileHandle->Read(testFileHandle,
                                      &readBufferSize,
                                      readBuffer);
        readBuffer[readBufferSize] = '\0';

        status = testFileHandle->Close(testFileHandle);
        if (EFI_ERROR(status))
            return status;


        DexprOS_Puts("File loading test, content of /EFI/BOOT/TESTFILE.TXT: ");
        DexprOS_Puts(readBuffer);
    }


    status = volumeRoot->Close(volumeRoot);
    if (EFI_ERROR(status))
        return status;
    */


    // Find the ACPI table
    /*const EFI_GUID acpiTableGuid = ACPI_20_TABLE_GUID;
    VOID* pAcpiTable = NULL;
    for (UINTN i = 0; i < pSystemTable->NumberOfTableEntries; ++i)
    {
        const EFI_GUID* pTableGuid = &pSystemTable->ConfigurationTable[i].VendorGuid;

        if (DexprOS_CompareUEFIGUID(pTableGuid, &acpiTableGuid))
        {
            pAcpiTable = pSystemTable->ConfigurationTable[i].VendorTable;
        }
    }
    // Make sure we have the ACPI table to read data from
    if (pAcpiTable == NULL)
        return EFI_OUT_OF_RESOURCES;*/


    // Claim memory ownership and exit boot services
    // EndBootPhase also automatically disables interrupts

    UINTN memoryMapSize = 0;
    UINTN memoryDescriptorSize = 0;
    UINT32 memoryDescriptorVersion = 0;

    UINTN memoryMapKey = 0;

    void* pMemoryMapBuffer = NULL;


    status = DexprOSBoot_EndBootPhase(imageHandle,
                                      pSystemTable,
                                      pGop->Mode->FrameBufferBase,
                                      pGop->Mode->FrameBufferSize,
                                      &pMemoryMapBuffer,
                                      &memoryMapSize,
                                      &memoryMapKey,
                                      &memoryDescriptorSize,
                                      &memoryDescriptorVersion);
    if (EFI_ERROR(status))
        return status;


    // Create (temporary) initial memory map

    DexprOS_InitialMemMap initialMemMap;
    if (!DexprOS_CreateInitialMemMapFromEfi(pMemoryMapBuffer,
                                            memoryMapSize,
                                            memoryDescriptorSize,
                                            memoryDescriptorVersion,
                                            &initialMemMap))
        return EFI_OUT_OF_RESOURCES;


    DexprOS_EnableMemoryProtectionCpuFeatures();
    // Fill global paging settings struct
    g_DexprOS_PagingSettings.pagingMode = DEXPROS_PAGING_MODE_4_LEVEL;
    if (DexprOS_CheckCpu5LevelPagingSupport() && DexprOS_Is5LevelPagingActive())
        g_DexprOS_PagingSettings.pagingMode = DEXPROS_PAGING_MODE_5_LEVEL;
    g_DexprOS_PagingSettings.noExecuteAvailable = DexprOS_CheckCpuHasNX();


    DexprOS_InitFloatingPointOperations();

    

    DexprOS_BaseStartupInfo startupInfo;
    startupInfo.initialMemMap = initialMemMap;
    startupInfo.pEfiSystemTable = pSystemTable;
    startupInfo.efiMemoryMap.memoryMapSize = memoryMapSize;
    startupInfo.efiMemoryMap.memoryDescriptorSize = memoryDescriptorSize;
    startupInfo.efiMemoryMap.memoryDescriptorVersion = memoryDescriptorVersion;
    startupInfo.efiMemoryMap.memoryMapKey = memoryMapKey;
    startupInfo.efiMemoryMap.pMemoryMapBuffer = pMemoryMapBuffer;

    startupInfo.framebuffer.presentationWidth = horizontalResolution;
    startupInfo.framebuffer.presentationHeight = verticalResolution;
    startupInfo.framebuffer.presentationPixelBytes = 4;
    startupInfo.framebuffer.presentationPixelStride = framebufferStride;
    startupInfo.framebuffer.framebufferPhysAddr = framebufferMem;
    startupInfo.framebuffer.framebufferVirtAddr = framebufferMem;
    startupInfo.framebuffer.mainFramebufferSize = framebufferSize;
    startupInfo.framebuffer.pRenderBackBufferMemory = pFramebufferBackBuffer;

    startupInfo.framebuffer.framebufferFormat = DEXPROS_GR_FRAMEBUFFER_FORMAT_Max;
    if (framebufferFormat == PixelRedGreenBlueReserved8BitPerColor)
        startupInfo.framebuffer.framebufferFormat = DEXPROS_GR_FRAMEBUFFER_FORMAT_RGB8_RESERVED8;
    else if (framebufferFormat == PixelBlueGreenRedReserved8BitPerColor)
        startupInfo.framebuffer.framebufferFormat = DEXPROS_GR_FRAMEBUFFER_FORMAT_BGR8_RESERVED8;


    DexprOS_KernelMapping kernelMapping = DexprOS_ChooseStartupVirtMapping(&initialMemMap,
                                                                           framebufferSize);

    status = RemapEfiAddressMap(pSystemTable, &startupInfo.efiMemoryMap, kernelMapping.efiOffset);
    if (EFI_ERROR(status))
        return status;
    

    // This function call doesn't return on success
    DexprOS_PerformStartupRemap(&startupInfo, &kernelMapping, DexprOS_KernelInit);

    return EFI_OUT_OF_RESOURCES;
}

/*
EFI_STATUS DexprOSBoot_FindBootloaderVolume(EFI_HANDLE imageHandle,
                                            EFI_SYSTEM_TABLE* pSystemTable,
                                            EFI_FILE_IO_INTERFACE** pVolumeIO)
{
    EFI_STATUS status = 0;


    EFI_GUID imageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID filesystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_LOADED_IMAGE* pImage = NULL;

    status = pSystemTable->BootServices->HandleProtocol(imageHandle,
                                                        &imageProtocolGuid,
                                                        (VOID**)&pImage);
    if (EFI_ERROR(status))
        return status;

    
    status = pSystemTable->BootServices->HandleProtocol(pImage->DeviceHandle,
                                                        &filesystemProtocolGuid,
                                                        (VOID**)pVolumeIO);
    if (EFI_ERROR(status))
        return status;
    

    return status;
}
*/



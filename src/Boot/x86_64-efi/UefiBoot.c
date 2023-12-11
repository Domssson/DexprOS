#include "DexprOS/Drivers/Graphics/CpuGraphicsDriver.h"
#include "DexprOS/Kernel/ErrorDisplay.h"
#include "DexprOS/Kernel/x86_64/Interrupts.h"
#include "DexprOS/Kernel/x86_64/TaskStateSegment.h"
#include "DexprOS/Kernel/x86_64/IdtCreator.h"
#include "DexprOS/Kernel/x86_64/GdtSetup.h"
#include "DexprOS/Kernel/x86_64/SyscallHandler.h"
#include "DexprOS/Drivers/PICDriver.h"
#include "DexprOS/Drivers/PS2ControllerDriver.h"
#include "DexprOS/Drivers/Keyboard/USKeyboardLayout.h"
#include "DexprOS/Drivers/Keyboard/PS2KeyboardDriver.h"
#include "DexprOS/Shell.h"

#include <efi.h>

#include <stdint.h>
#include <stddef.h>


inline static int DexprOS_CompareUEFIGUID(const EFI_GUID* pGuid0,
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
}


static EFI_STATUS DexprOSBoot_GetMemoryMap(EFI_SYSTEM_TABLE* pSystemTable,
                                           void** pMemoryMapBuffer,
                                           UINTN* pMemoryMapSize,
                                           UINTN* pMemoryMapKey,
                                           UINTN* pMemoryDescriptorSize,
                                           UINT32* pMemoryDescriptorVersion);

static EFI_STATUS DexprOSBoot_EndBootPhase(EFI_HANDLE imageHandle,
                                           EFI_SYSTEM_TABLE* pSystemTable,
                                           void** pOutMemoryMapBuffer,
                                           UINTN* pOutMemoryMapSize,
                                           UINTN* pOutMemoryMapKey,
                                           UINTN* pOutMemoryDescriptorSize,
                                           UINT32* pOutMemoryDescriptorVersion);


static DexprOS_GraphicsDriver g_graphicsDriver = {0};

static EFI_SYSTEM_TABLE* g_pUefiSystemTable = NULL;

DexprOS_Shell g_shell;


EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* pSystemTable)
{
    EFI_STATUS status = 0;


    g_pUefiSystemTable = pSystemTable;


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

    if (DexprOS_InitCpuGraphicsDriver(&g_graphicsDriver,
                                      pGop,
                                      pSystemTable->BootServices) != DEXPROS_CPU_GRAPHICS_DRV_INIT_SUCCESS)
        return EFI_OUT_OF_RESOURCES;

    DexprOS_InitKernelErrorDisplay(&g_graphicsDriver,
                                   horizontalResolution);
    
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

    UINTN memoryMapSize = 0;
    UINTN memoryDescriptorSize = 0;
    UINT32 memoryDescriptorVersion = 0;

    UINTN memoryMapKey = 0;

    void* pMemoryMapBuffer = NULL;

    status = DexprOSBoot_EndBootPhase(imageHandle,
                                      pSystemTable,
                                      &pMemoryMapBuffer,
                                      &memoryMapSize,
                                      &memoryMapKey,
                                      &memoryDescriptorSize,
                                      &memoryDescriptorVersion);
    if (EFI_ERROR(status))
        return status;


    DexprOS_DisableInterrupts();

    uint64_t tssAdresses[1];
    uint16_t tssSizes[1];
    DexprOS_TaskStateSegment taskStateSegments[1] = {0};
    DexprOS_SetupTaskStateSegments(taskStateSegments,
                                   tssAdresses,
                                   tssSizes,
                                   1);
    uint16_t kernelCodeSegmentOffset = 0;
    uint16_t userBaseSegmentOffset = 0;
    uint16_t tssSegmentOffsets[1];

    DexprOS_SetupGDT(tssAdresses, tssSizes, 1,
                     &kernelCodeSegmentOffset,
                     &userBaseSegmentOffset,
                     tssSegmentOffsets);

    DexprOS_SetupIDT(kernelCodeSegmentOffset);
    DexprOS_EnableSyscallExtension(kernelCodeSegmentOffset,
                                   userBaseSegmentOffset);

    DexprOS_InitialisePIC(32, 40);

    DexprOS_PS2ControllerInitResult ps2Result = DexprOS_InitialisePS2Controller();
    if (ps2Result.hasKeyboard)
        DexprOS_InitialisePS2KeyboardDriver(DexprOS_GetKeyboardLayout_US());


    DexprOS_CreateShell(&g_shell,
                        pSystemTable,
                        &g_graphicsDriver,
                        horizontalResolution,
                        verticalResolution);


    DexprOS_EnableInterrupts();


    DexprOS_ShellPuts(&g_shell, "Welcome to DexprOS!\n\n");
    DexprOS_ShellPuts(&g_shell, "DexprOS version: pre-release 0.1.0\n");
    DexprOS_ShellPuts(&g_shell, "If you don't know what to do, type \"help\" and hit Enter.\n\n");

    DexprOS_ShellActivatePrompt(&g_shell);


    while (1)
        __asm__ volatile("hlt");


    DexprOS_DestroyShell(&g_shell);

    // Unload the graphics driver
    DexprOS_DestroyCpuGraphicsDriver(&g_graphicsDriver);

    return EFI_SUCCESS;
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


EFI_STATUS DexprOSBoot_GetMemoryMap(EFI_SYSTEM_TABLE* pSystemTable,
                                    void** pMemoryMapBuffer,
                                    UINTN* pMemoryMapSize,
                                    UINTN* pMemoryMapKey,
                                    UINTN* pMemoryDescriptorSize,
                                    UINT32* pMemoryDescriptorVersion)
{
    EFI_STATUS status = 0;

    if (*pMemoryMapSize == 0)
        *pMemoryMapSize = 512;

    do
    {
        if (*pMemoryMapBuffer != NULL)
        {
            status = pSystemTable->BootServices->FreePool(*pMemoryMapBuffer);
            if (EFI_ERROR(status))
                return status;
        }

        status = pSystemTable->BootServices->AllocatePool(EfiLoaderData, *pMemoryMapSize, pMemoryMapBuffer);
        if (EFI_ERROR(status))
            return status;

        status = pSystemTable->BootServices->GetMemoryMap(pMemoryMapSize,
                                                          (EFI_MEMORY_DESCRIPTOR*)*pMemoryMapBuffer,
                                                          pMemoryMapKey,
                                                          pMemoryDescriptorSize,
                                                          pMemoryDescriptorVersion);
    }
    while (status == EFI_BUFFER_TOO_SMALL);

    return status;
}

EFI_STATUS DexprOSBoot_EndBootPhase(EFI_HANDLE imageHandle,
                                    EFI_SYSTEM_TABLE* pSystemTable,
                                    void** pOutMemoryMapBuffer,
                                    UINTN* pOutMemoryMapSize,
                                    UINTN* pOutMemoryMapKey,
                                    UINTN* pOutMemoryDescriptorSize,
                                    UINT32* pOutMemoryDescriptorVersion)
{
    EFI_STATUS status = 0;

    void* pMemoryBuffer = NULL;
    UINTN memoryMapSize = 0;
    UINTN memoryMapKey = 0;

    do
    {
        status = DexprOSBoot_GetMemoryMap(pSystemTable,
                                          &pMemoryBuffer,
                                          &memoryMapSize,
                                          &memoryMapKey,
                                          pOutMemoryDescriptorSize,
                                          pOutMemoryDescriptorVersion);
        if (EFI_ERROR(status))
            return status;

        status = pSystemTable->BootServices->ExitBootServices(imageHandle,
                                                              memoryMapKey);
    } while (status == EFI_INVALID_PARAMETER);

    if (EFI_ERROR(status))
        return status;

    *pOutMemoryMapBuffer = pMemoryBuffer;
    *pOutMemoryMapSize = memoryMapSize;
    *pOutMemoryMapKey = memoryMapKey;

    
    pSystemTable->ConsoleInHandle = NULL;
    pSystemTable->ConIn = NULL;
    pSystemTable->ConsoleOutHandle = NULL;
    pSystemTable->ConOut = NULL;
    pSystemTable->StandardErrorHandle = NULL;
    pSystemTable->StdErr = NULL;
    pSystemTable->BootServices = NULL;

    return status;
}



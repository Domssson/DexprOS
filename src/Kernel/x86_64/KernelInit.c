#include "DexprOS/Kernel/x86_64/KernelInit.h"

#include "DexprOS/Kernel/x86_64/Interrupts.h"
#include "DexprOS/Kernel/x86_64/TaskStateSegment.h"
#include "DexprOS/Kernel/x86_64/IdtCreator.h"
#include "DexprOS/Kernel/x86_64/GdtSetup.h"
#include "DexprOS/Kernel/x86_64/SyscallHandler.h"
#include "DexprOS/Kernel/x86_64/CpuFeatures.h"
#include "DexprOS/Kernel/x86_64/PageMapSwitching.h"
#include "DexprOS/Kernel/x86_64/PagingSettings.h"
#include "DexprOS/Drivers/PICDriver.h"
#include "DexprOS/Drivers/PS2ControllerDriver.h"
#include "DexprOS/Drivers/Keyboard/USKeyboardLayout.h"
#include "DexprOS/Drivers/Keyboard/PS2KeyboardDriver.h"
#include "DexprOS/Drivers/Graphics/CpuGraphicsDriver.h"
#include "DexprOS/Kernel/ErrorDisplay.h"
#include "DexprOS/Shell.h"


static DexprOS_CpuGrGraphicsDrvData g_cpuGraphicsDriverData = {0};
static DexprOS_GraphicsDriver g_graphicsDriver = {0};


DexprOS_Shell g_shell;


static void testDisplayUint64Hex(uint64_t value);


void DexprOS_KernelInit(const DexprOS_KernelStartInfo* pStartInfo)
{
    DexprOS_VirtualMemoryAddress rellocOffset = pStartInfo->kernelMapping.kernelOffset - pStartInfo->initialMemMap.virtualMapOffset;
    
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

    DexprOS_SetupIDT(rellocOffset,
                     kernelCodeSegmentOffset);


    DexprOS_EnableSyscallExtension(kernelCodeSegmentOffset,
                                   userBaseSegmentOffset);


    DexprOS_InitialisePIC(32, 40);


    DexprOS_PS2ControllerInitResult ps2Result = DexprOS_InitialisePS2Controller();
    if (ps2Result.hasKeyboard)
        DexprOS_InitialisePS2KeyboardDriver(DexprOS_GetKeyboardLayout_US(),
                                            rellocOffset);


    DexprOS_InitCpuGraphicsDriver(&g_graphicsDriver,
                                  &g_cpuGraphicsDriverData,
                                  &pStartInfo->framebuffer,
                                  rellocOffset);

    DexprOS_InitKernelErrorDisplay(&g_graphicsDriver,
                                   pStartInfo->framebuffer.presentationWidth);
    

    DexprOS_CreateShell(&g_shell,
                        pStartInfo->pEfiSystemTable,
                        &g_graphicsDriver,
                        pStartInfo->framebuffer.presentationWidth,
                        pStartInfo->framebuffer.presentationHeight,
                        rellocOffset);


    DexprOS_EnableInterrupts();


    DexprOS_ShellPuts(&g_shell, "Welcome to DexprOS!\n\n");
    DexprOS_ShellPuts(&g_shell, "DexprOS version: pre-release 0.1.2\n");
    DexprOS_ShellPuts(&g_shell, "If you don't know what to do, type \"help\" and hit Enter.\n\n");


    if (DexprOS_CheckCpu5LevelPagingSupport())
    {
        DexprOS_ShellPuts(&g_shell, "5-level paging supported!\n\n");

        if (g_DexprOS_PagingSettings.pagingMode == DEXPROS_PAGING_MODE_5_LEVEL)
            DexprOS_ShellPuts(&g_shell, "5-level paging active!\n\n");
    }
    

    DexprOS_ShellPuts(&g_shell, "Initial memory map address: ");
    testDisplayUint64Hex((uint64_t)pStartInfo->initialMemMap.pEntries);
    DexprOS_ShellPuts(&g_shell, "\nNum initial memory map entries: ");
    testDisplayUint64Hex((uint64_t)pStartInfo->initialMemMap.numEntries);


    DexprOS_KernelMapping kernelMapping = pStartInfo->kernelMapping;

    DexprOS_ShellPuts(&g_shell, "\n\nKernel space start: ");
    testDisplayUint64Hex(kernelMapping.kernelSpaceStart);
    DexprOS_ShellPuts(&g_shell, "\nKernel mapping offset: ");
    testDisplayUint64Hex(kernelMapping.kernelOffset);
    DexprOS_ShellPuts(&g_shell, ", size: ");
    testDisplayUint64Hex(kernelMapping.kernelMapSize);
    DexprOS_ShellPuts(&g_shell, "\nPage dirs mapping offset: ");
    testDisplayUint64Hex(kernelMapping.pageDirectoriesOffset);
    DexprOS_ShellPuts(&g_shell, ", size: ");
    testDisplayUint64Hex(kernelMapping.pageDirectoriesSize);
    DexprOS_ShellPuts(&g_shell, "\nEfi mapping offset: ");
    testDisplayUint64Hex(kernelMapping.efiOffset);
    DexprOS_ShellPuts(&g_shell, ", size: ");
    testDisplayUint64Hex(kernelMapping.efiSize);
    DexprOS_ShellPuts(&g_shell, "\nStack space offset: ");
    testDisplayUint64Hex(kernelMapping.stackOffset);
    DexprOS_ShellPuts(&g_shell, ", reserved size: ");
    testDisplayUint64Hex(kernelMapping.reservedStackSpace);
    DexprOS_ShellPuts(&g_shell, "\nFramebuffer offset: ");
    testDisplayUint64Hex(kernelMapping.framebufferOffset);
    DexprOS_ShellPuts(&g_shell, ", size: ");
    testDisplayUint64Hex(kernelMapping.framebufferSize);



    DexprOS_ShellPuts(&g_shell, "\n\n");
    

    DexprOS_ShellActivatePrompt(&g_shell);


    while (1)
        __asm__ volatile("hlt");


    //DexprOS_DestroyShell(&g_shell);

    // Unload the graphics driver
    //DexprOS_DestroyCpuGraphicsDriver(&g_graphicsDriver);
}


void testDisplayUint64Hex(uint64_t value)
{
    DexprOS_ShellPuts(&g_shell, "0x");

    for (int i = 0; i < 16; ++i)
    {
        uint8_t singleCharValue = ((value >> (60 - i * 4)) & 0xF);

        switch (singleCharValue)
        {
        case 0:
            DexprOS_ShellPutChar(&g_shell, '0');
            break;
        case 1:
            DexprOS_ShellPutChar(&g_shell, '1');
            break;
        case 2:
            DexprOS_ShellPutChar(&g_shell, '2');
            break;
        case 3:
            DexprOS_ShellPutChar(&g_shell, '3');
            break;
        case 4:
            DexprOS_ShellPutChar(&g_shell, '4');
            break;
        case 5:
            DexprOS_ShellPutChar(&g_shell, '5');
            break;
        case 6:
            DexprOS_ShellPutChar(&g_shell, '6');
            break;
        case 7:
            DexprOS_ShellPutChar(&g_shell, '7');
            break;
        case 8:
            DexprOS_ShellPutChar(&g_shell, '8');
            break;
        case 9:
            DexprOS_ShellPutChar(&g_shell, '9');
            break;
        case 10:
            DexprOS_ShellPutChar(&g_shell, 'A');
            break;
        case 11:
            DexprOS_ShellPutChar(&g_shell, 'B');
            break;
        case 12:
            DexprOS_ShellPutChar(&g_shell, 'C');
            break;
        case 13:
            DexprOS_ShellPutChar(&g_shell, 'D');
            break;
        case 14:
            DexprOS_ShellPutChar(&g_shell, 'E');
            break;
        case 15:
            DexprOS_ShellPutChar(&g_shell, 'F');
            break;
        
        default:
            break;
        }
    }
}


/*static const char* MemTypeToString(DexprOS_PhysicalMemoryType type)
{
    switch (type)
    {
    case DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE_PERSISTENT:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_PERSISTENT";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA";
    case DEXPROS_PHYSICAL_MEMORY_TYPE_Max:
        return "DEXPROS_PHYSICAL_MEMORY_TYPE_Max";
    }
    return "DEXPROS_PHYSICAL_MEMORY_TYPE_Max";
}*/
/*static const char* InitMemUsageToString(DexprOS_InitialMemMapMappedUsage usage)
{
    switch (usage)
    {
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE";
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_AVAILABLE:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_AVAILABLE";
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_CODE:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_CODE";
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA";
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_CODE:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_CODE";
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_DATA:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_DATA";
    case DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_INITIAL_MEMMAP:
        return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_INITIAL_MEMMAP";
    }
    return "DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE";
}*/



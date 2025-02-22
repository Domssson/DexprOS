#ifndef DEXPROS_KERNEL_x86_64_KERNELSTARTINFO_H_INCLUDED
#define DEXPROS_KERNEL_x86_64_KERNELSTARTINFO_H_INCLUDED

#include "../Memory/MemoryDef.h"
#include "../Memory/InitialMemMap.h"
#include "../Memory/InitialLinearAllocator.h"

#include "DexprOS/Drivers/Graphics/GraphicsDriver.h"

#include <efi.h>

#include <stdint.h>
#include <stddef.h>



typedef struct DexprOS_KernelMapping
{
    DexprOS_VirtualMemoryAddress kernelSpaceStart;

    DexprOS_VirtualMemoryAddress kernelOffset;
    size_t kernelMapSize;

    DexprOS_VirtualMemoryAddress pageDirectoriesOffset;
    size_t pageDirectoriesSize;

    DexprOS_VirtualMemoryAddress efiOffset;
    size_t efiSize;

    DexprOS_VirtualMemoryAddress stackOffset;
    size_t reservedStackSpace;

    DexprOS_VirtualMemoryAddress framebufferOffset;
    size_t framebufferSize;
} DexprOS_KernelMapping;


typedef struct DexprOS_EfiMemoryMap
{
    size_t memoryMapSize;
    size_t memoryDescriptorSize;
    uint32_t memoryDescriptorVersion;

    size_t memoryMapKey;

    void* pMemoryMapBuffer;
} DexprOS_EfiMemoryMap;


typedef struct DexprOS_KernelStartInfo
{
    DexprOS_KernelMapping kernelMapping;

    DexprOS_InitialMemMap initialMemMap;

    EFI_SYSTEM_TABLE* pEfiSystemTable;

    DexprOS_EfiMemoryMap efiMemoryMap;

    DexprOS_StartupFramebufferInfo framebuffer;

    DexprOS_VirtualMemoryAddress kernelStackTop;
    DexprOS_VirtualMemoryAddress kernelStackBottom;

    DexprOS_VirtualMemoryAddress startInfoBufferStart;
    DexprOS_VirtualMemoryAddress startInfoBufferEnd;

    DexprOS_VirtualMemoryAddress remapCodeBufferStart;
    DexprOS_VirtualMemoryAddress remapCodeBufferEnd;

    DexprOS_VirtualMemoryAddress framebufferStart;
    DexprOS_VirtualMemoryAddress framebufferEnd;
} DexprOS_KernelStartInfo;


#endif // DEXPROS_KERNEL_x86_64_KERNELSTARTINFO_H_INCLUDED

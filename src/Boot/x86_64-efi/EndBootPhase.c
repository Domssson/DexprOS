#include "DexprOS/Boot/x86_64-efi/EndBootPhase.h"

#include "DexprOS/Boot/x86_64-efi/InitialPageMapSetup.h"
#include "DexprOS/Boot/x86_64-efi/InitialPageMapSwitch.h"

#include "DexprOS/Kernel/kstdlib/string.h"

#include <stdbool.h>


// a must be power of 2
#define DEXPROSBOOT_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))


typedef enum DexprOSBoot_PageMapSetupType
{
    // 4-level page -map preparation
    DEXPROSBOOT_PAGE_MAP_4_SETUP,
    // 5-level page map preparation
    DEXPROSBOOT_PAGE_MAP_5_SETUP,
    // transition from 4-level paging mode to 5-level paging mode, the most
    // complex setup type
    DEXPROSBOOT_PAGE_MAP_4_TO_5_TRANSITION
} DexprOSBoot_PageMapSetupType;


typedef struct DexprOSBoot_PageMap4To5TransitionData
{
    size_t tmpPageMap4BufferOffset;
    size_t tmpPageMap4BufferSize;

    size_t codeRellocBufferOffset;
    size_t codeRellocBufferSize;

    size_t codeScratchBufferOffset;
    size_t codeScratchBufferSize;
} DexprOSBoot_PageMap4To5TransitionData;


typedef struct DexprOSBoot_PageMapSetupBuffer
{
    void* pBuffer;
    size_t bufferSize;

    size_t pageMapOffset;
    size_t pageMapSize;

    DexprOSBoot_PageMap4To5TransitionData transitionData;
} DexprOSBoot_PageMapSetupBuffer;



static EFI_STATUS GetMemoryMap(EFI_SYSTEM_TABLE* pSystemTable,
                               void** pMemoryMapBuffer,
                               UINTN* pMemoryMapSize,
                               UINTN* pMemoryMapKey,
                               UINTN* pMemoryDescriptorSize,
                               UINT32* pMemoryDescriptorVersion)
{
    EFI_STATUS status = 0;

    if (*pMemoryMapSize == 0)
        *pMemoryMapSize = 512;

    for (;;)
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

        if (status == EFI_BUFFER_TOO_SMALL)
            *pMemoryMapSize += (*pMemoryDescriptorSize) * 2;
        else
            return status;
    }
}




static void FillPageMap4Sizes(DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer,
                              UINTN* pRequiredBufferSize,
                              void* memoryMapBuffer,
                              UINTN memoryMapSize,
                              UINTN memoryDescriptorSize,
                              UINT32 memoryDescriptorVersion,
                              EFI_PHYSICAL_ADDRESS framebufferBase,
                              UINTN framebufferSize)
{
    UINTN mapSize = DexprOSBoot_CalculateInitialPageMap4Size(memoryMapBuffer,
                                                             memoryMapSize,
                                                             memoryDescriptorSize,
                                                             memoryDescriptorVersion,
                                                             framebufferBase,
                                                             framebufferSize);
    pPageMapBuffer->pageMapOffset = 0;
    pPageMapBuffer->pageMapSize = mapSize;
    *pRequiredBufferSize = mapSize;
}


static void FillPageMap5Sizes(DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer,
                              UINTN* pRequiredBufferSize,
                              void* memoryMapBuffer,
                              UINTN memoryMapSize,
                              UINTN memoryDescriptorSize,
                              UINT32 memoryDescriptorVersion,
                              EFI_PHYSICAL_ADDRESS framebufferBase,
                              UINTN framebufferSize)
{
    UINTN mapSize = DexprOSBoot_CalculateInitialPageMap5Size(memoryMapBuffer,
                                                             memoryMapSize,
                                                             memoryDescriptorSize,
                                                             memoryDescriptorVersion,
                                                             framebufferBase,
                                                             framebufferSize);
    pPageMapBuffer->pageMapOffset = 0;
    pPageMapBuffer->pageMapSize = mapSize;
    *pRequiredBufferSize = mapSize;
}


static void FillPageMap4To5SizesBelow4G(DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer,
                                        UINTN* pRequiredBufferSize,
                                        void* memoryMapBuffer,
                                        UINTN memoryMapSize,
                                        UINTN memoryDescriptorSize,
                                        UINT32 memoryDescriptorVersion,
                                        EFI_PHYSICAL_ADDRESS framebufferBase,
                                        UINTN framebufferSize)
{
    size_t totalSize = 0;


    // No code rellocation is needed. We can just setup a 5-level map

    size_t level5Size = DexprOSBoot_CalculateInitialPageMap5Size(memoryMapBuffer,
                                                                 memoryMapSize,
                                                                 memoryDescriptorSize,
                                                                 memoryDescriptorVersion,
                                                                 framebufferBase,
                                                                 framebufferSize);
    pPageMapBuffer->pageMapOffset = 0;
    pPageMapBuffer->pageMapSize = level5Size;

    totalSize += level5Size;
    totalSize = DEXPROSBOOT_ALIGN(totalSize,
                                  DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_ALIGN);

    // We also need a scratch buffer for the function that switches
    // paging modes

    pPageMapBuffer->transitionData.codeScratchBufferOffset = totalSize;
    pPageMapBuffer->transitionData.codeScratchBufferSize = DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE;
    totalSize += DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE;

    *pRequiredBufferSize = totalSize;
}

static void FillPageMap4To5SizesAbove4G(DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer,
                                        UINTN* pRequiredBufferSize,
                                        void* memoryMapBuffer,
                                        UINTN memoryMapSize,
                                        UINTN memoryDescriptorSize,
                                        UINT32 memoryDescriptorVersion,
                                        EFI_PHYSICAL_ADDRESS framebufferBase,
                                        UINTN framebufferSize)
{
    size_t totalSize = 0;


    size_t level5Size = DexprOSBoot_CalculateInitialPageMap5Size(memoryMapBuffer,
                                                                 memoryMapSize,
                                                                 memoryDescriptorSize,
                                                                 memoryDescriptorVersion,
                                                                 framebufferBase,
                                                                 framebufferSize);
    pPageMapBuffer->pageMapOffset = 0;
    pPageMapBuffer->pageMapSize = level5Size;

    totalSize += level5Size;
    totalSize = DEXPROSBOOT_ALIGN(totalSize, DEXPROSBOOT_INITIAL_PAGE_SIZE);


    // We also need a temporary 4-level page map with full privilages
    // on EfiLoaderData as we need to rellocate some code to some memory
    // region below 4 GiB and execute it.

    size_t level4Size = DexprOSBoot_CalculateTransitionalPageMap4Size(memoryMapBuffer,
                                                                      memoryMapSize,
                                                                      memoryDescriptorSize,
                                                                      memoryDescriptorVersion);

    pPageMapBuffer->transitionData.tmpPageMap4BufferOffset = totalSize;
    pPageMapBuffer->transitionData.tmpPageMap4BufferSize = level4Size;

    totalSize += level4Size;
    totalSize = DEXPROSBOOT_ALIGN(totalSize, DEXPROSBOOT_INITIAL_PAGE_SIZE);


    // Buffer for rellocation of the code that switches the page maps. The code
    // needs to be rellocated as it temporarily switches to 32-bit protected
    // mode which can only access the first 4 GiBs of RAM.

    size_t codeRellocSize = (size_t)DexprOSBoot_32BitPageMap5SwitchRegionEnd -
                            (size_t)DexprOSBoot_32BitPageMap5SwitchRegionStart;

    pPageMapBuffer->transitionData.codeRellocBufferOffset = totalSize;
    pPageMapBuffer->transitionData.codeRellocBufferSize = codeRellocSize;
    
    totalSize += codeRellocSize;
    totalSize = DEXPROSBOOT_ALIGN(totalSize,
                                  DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_ALIGN);


    // We also need a scratch buffer for the function that switches
    // paging modes

    pPageMapBuffer->transitionData.codeScratchBufferOffset = totalSize;
    pPageMapBuffer->transitionData.codeScratchBufferSize = DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE;
    totalSize += DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE;

    *pRequiredBufferSize = totalSize;
}


static void FillPageMapSizes(DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer,
                             UINTN* pRequiredBufferSize,
                             void* memoryMapBuffer,
                             UINTN memoryMapSize,
                             UINTN memoryDescriptorSize,
                             UINT32 memoryDescriptorVersion,
                             EFI_PHYSICAL_ADDRESS framebufferBase,
                             UINTN framebufferSize,
                             DexprOSBoot_PageMapSetupType pageMapSetupType)
{
    switch (pageMapSetupType)
    {
    case DEXPROSBOOT_PAGE_MAP_4_SETUP:
        FillPageMap4Sizes(pPageMapBuffer,
                          pRequiredBufferSize,
                          memoryMapBuffer,
                          memoryMapSize,
                          memoryDescriptorSize,
                          memoryDescriptorVersion,
                          framebufferBase,
                          framebufferSize);
        break;
    
    case DEXPROSBOOT_PAGE_MAP_5_SETUP:
        FillPageMap5Sizes(pPageMapBuffer,
                          pRequiredBufferSize,
                          memoryMapBuffer,
                          memoryMapSize,
                          memoryDescriptorSize,
                          memoryDescriptorVersion,
                          framebufferBase,
                          framebufferSize);
        break;
    
    case DEXPROSBOOT_PAGE_MAP_4_TO_5_TRANSITION:

        // Here we branch as some code may need to be rellocated or not
        // depending on the memory region it's placed in

        if ((size_t)DexprOSBoot_32BitPageMap5SwitchRegionEnd < 4294967296) // 4 GiB
        {
            FillPageMap4To5SizesBelow4G(pPageMapBuffer,
                                        pRequiredBufferSize,
                                        memoryMapBuffer,
                                        memoryMapSize,
                                        memoryDescriptorSize,
                                        memoryDescriptorVersion,
                                        framebufferBase,
                                        framebufferSize);
        }
        else
        {
            FillPageMap4To5SizesAbove4G(pPageMapBuffer,
                                        pRequiredBufferSize,
                                        memoryMapBuffer,
                                        memoryMapSize,
                                        memoryDescriptorSize,
                                        memoryDescriptorVersion,
                                        framebufferBase,
                                        framebufferSize);
        }
        break;
    }
}


static EFI_STATUS ReAllocPageMapBuffer(EFI_SYSTEM_TABLE* pSystemTable,
                                       EFI_PHYSICAL_ADDRESS* pBufferAddress,
                                       UINTN numPreviousPages,
                                       UINTN numNewPages,
                                       DexprOSBoot_PageMapSetupType pageMapSetupType)
{
    EFI_STATUS status = 0;


    if (numPreviousPages != 0)
    {
        status = pSystemTable->BootServices->FreePages(*pBufferAddress,
                                                       numPreviousPages);
        if (EFI_ERROR(status))
            return status;
    }


    switch (pageMapSetupType)
    {
    case DEXPROSBOOT_PAGE_MAP_4_SETUP:
    case DEXPROSBOOT_PAGE_MAP_5_SETUP:
    
        status = pSystemTable->BootServices->AllocatePages(AllocateAnyPages,
                                                           EfiLoaderData,
                                                           numNewPages,
                                                           pBufferAddress);
        break;
    
    case DEXPROSBOOT_PAGE_MAP_4_TO_5_TRANSITION:
        
        // Make sure the max. allocated address for the page map buffer is
        // lower than 4 GiB as we want to switch to 5-level paging and this
        // involves switching to 32-bit protected mode
        *pBufferAddress = 4294967296; // 4 GiB
        status = pSystemTable->BootServices->AllocatePages(AllocateMaxAddress,
                                                           EfiLoaderData,
                                                           numNewPages,
                                                           pBufferAddress);
        break;
    }
    
    return status;
}


static EFI_STATUS GetMemoryMapAndPageMapBuffer(EFI_SYSTEM_TABLE* pSystemTable,
                                               void** pMemoryMapBuffer,
                                               UINTN* pMemoryMapSize,
                                               UINTN* pMemoryMapKey,
                                               UINTN* pMemoryDescriptorSize,
                                               UINT32* pMemoryDescriptorVersion,
                                               EFI_PHYSICAL_ADDRESS framebufferBase,
                                               UINTN framebufferSize,
                                               DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer,
                                               DexprOSBoot_PageMapSetupType pageMapSetupType)
{
    EFI_STATUS status = 0;

    UINTN numPreviousPages = (pPageMapBuffer->bufferSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE;

    UINTN numNewEfiPages = numPreviousPages;
    if (numNewEfiPages == 0)
        numNewEfiPages = 4;

    
    for (;;)
    {
        EFI_PHYSICAL_ADDRESS bufferAddress = (EFI_PHYSICAL_ADDRESS)pPageMapBuffer->pBuffer;


        ReAllocPageMapBuffer(pSystemTable,
                             &bufferAddress,
                             numPreviousPages,
                             numNewEfiPages,
                             pageMapSetupType);
        if (EFI_ERROR(status))
            return status;


        status = GetMemoryMap(pSystemTable,
                              pMemoryMapBuffer,
                              pMemoryMapSize,
                              pMemoryMapKey,
                              pMemoryDescriptorSize,
                              pMemoryDescriptorVersion);
        if (EFI_ERROR(status))
            return status;


        UINTN requiredPageMapSize = 0;
        FillPageMapSizes(pPageMapBuffer,
                         &requiredPageMapSize,
                         *pMemoryMapBuffer,
                         *pMemoryMapSize,
                         *pMemoryDescriptorSize,
                         *pMemoryDescriptorVersion,
                         framebufferBase,
                         framebufferSize,
                         pageMapSetupType);


        pPageMapBuffer->pBuffer = (void*)bufferAddress;
        pPageMapBuffer->bufferSize = numNewEfiPages * EFI_PAGE_SIZE;

        // The allocated buffer has the size we need for the new page map!
        if (pPageMapBuffer->bufferSize >= requiredPageMapSize)
            return EFI_SUCCESS;

        // Otherwise make the new buffer two times bigger than the requested
        // size as the size we need reported by memory map may increase
        // at the second attempt
        UINTN requiredPages = (requiredPageMapSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE;
        numPreviousPages = numNewEfiPages;
        numNewEfiPages = requiredPages * 2;
    }
}


static EFI_STATUS ExitBootServicesAndGetBuffer(EFI_HANDLE imageHandle,
                                               EFI_SYSTEM_TABLE* pSystemTable,
                                               EFI_PHYSICAL_ADDRESS framebufferBase,
                                               UINTN framebufferSize,
                                               DexprOSBoot_PageMapSetupType pageMapSetupType,
                                               void** pOutMemoryMapBuffer,
                                               UINTN* pOutMemoryMapSize,
                                               UINTN* pOutMemoryMapKey,
                                               UINTN* pOutMemoryDescriptorSize,
                                               UINT32* pOutMemoryDescriptorVersion,
                                               DexprOSBoot_PageMapSetupBuffer* pOutPageMapBuffer)
{
    EFI_STATUS status = 0;

    void* pMemoryBuffer = NULL;
    UINTN memoryMapSize = 0;
    UINTN memoryMapKey = 0;

    DexprOSBoot_PageMapSetupBuffer pageMapBuffer;
    pageMapBuffer.pBuffer = NULL;
    pageMapBuffer.bufferSize = 0;
    pageMapBuffer.pageMapOffset = 0;
    pageMapBuffer.pageMapSize = 0;
    pageMapBuffer.transitionData.tmpPageMap4BufferOffset = 0;
    pageMapBuffer.transitionData.tmpPageMap4BufferSize = 0;
    pageMapBuffer.transitionData.codeRellocBufferOffset = 0;
    pageMapBuffer.transitionData.codeRellocBufferSize = 0;
    pageMapBuffer.transitionData.codeScratchBufferOffset = 0;
    pageMapBuffer.transitionData.codeScratchBufferSize = 0;

    do
    {
        status = GetMemoryMapAndPageMapBuffer(pSystemTable,
                                              &pMemoryBuffer,
                                              &memoryMapSize,
                                              &memoryMapKey,
                                              pOutMemoryDescriptorSize,
                                              pOutMemoryDescriptorVersion,
                                              framebufferBase,
                                              framebufferSize,
                                              &pageMapBuffer,
                                              pageMapSetupType);
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
    *pOutPageMapBuffer = pageMapBuffer;

    
    pSystemTable->ConsoleInHandle = NULL;
    pSystemTable->ConIn = NULL;
    pSystemTable->ConsoleOutHandle = NULL;
    pSystemTable->ConOut = NULL;
    pSystemTable->StandardErrorHandle = NULL;
    pSystemTable->StdErr = NULL;
    pSystemTable->BootServices = NULL;

    return status;
}


static bool CreateAndSwitchPageMap4(void* pMemoryMapBuffer,
                                    UINTN memoryMapSize,
                                    UINTN memoryDescriptorSize,
                                    UINT32 memoryDescriptorVersion,
                                    EFI_PHYSICAL_ADDRESS framebufferBase,
                                    UINTN framebufferSize,
                                    const DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer)
{
    char* pPageMapMemory = (char*)pPageMapBuffer->pBuffer +
                           pPageMapBuffer->pageMapOffset;

    void* pPML = DexprOSBoot_SetupInitialPageMap4(pMemoryMapBuffer,
                                                  memoryMapSize,
                                                  memoryDescriptorSize,
                                                  memoryDescriptorVersion,
                                                  framebufferBase,
                                                  framebufferSize,
                                                  (void*)pPageMapMemory,
                                                  pPageMapBuffer->pageMapSize);
    if (pPML == NULL)
        return false;
    
    DexprOSBoot_SwitchPageMap((uint64_t)pPML);

    return true;
}


static bool CreateAndSwitchPageMap5(void* pMemoryMapBuffer,
                                    UINTN memoryMapSize,
                                    UINTN memoryDescriptorSize,
                                    UINT32 memoryDescriptorVersion,
                                    EFI_PHYSICAL_ADDRESS framebufferBase,
                                    UINTN framebufferSize,
                                    const DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer)
{
    char* pPageMapMemory = (char*)pPageMapBuffer->pBuffer +
                           pPageMapBuffer->pageMapOffset;

    void* pPML = DexprOSBoot_SetupInitialPageMap5(pMemoryMapBuffer,
                                                  memoryMapSize,
                                                  memoryDescriptorSize,
                                                  memoryDescriptorVersion,
                                                  framebufferBase,
                                                  framebufferSize,
                                                  (void*)pPageMapMemory,
                                                  pPageMapBuffer->pageMapSize);
    if (pPML == NULL)
        return false;
    
    DexprOSBoot_SwitchPageMap((uint64_t)pPML);

    return true;
}


static bool CreateAndSwitchPageMap4To5Below4G(void* pMemoryMapBuffer,
                                              UINTN memoryMapSize,
                                              UINTN memoryDescriptorSize,
                                              UINT32 memoryDescriptorVersion,
                                              EFI_PHYSICAL_ADDRESS framebufferBase,
                                              UINTN framebufferSize,
                                              const DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer)
{
    // We don't need to rellocate any code as it's already in the first 4 GiBs
    // of RAM, so just setup the 5-level memory map.
    // The buffer we have is guaranteed to be in this region thanks to the
    // allocating function.

    char* pPageMapMemory = (char*)pPageMapBuffer->pBuffer +
                           pPageMapBuffer->pageMapOffset;

    void* pPML = DexprOSBoot_SetupInitialPageMap5(pMemoryMapBuffer,
                                                  memoryMapSize,
                                                  memoryDescriptorSize,
                                                  memoryDescriptorVersion,
                                                  framebufferBase,
                                                  framebufferSize,
                                                  (void*)pPageMapMemory,
                                                  pPageMapBuffer->pageMapSize);
    if (pPML == NULL)
        return false;
    
    // Get the switching function address without rellocation
    size_t funcStartAddress = (size_t)DexprOSBoot_32BitPageMap5SwitchRegionStart;
    DexprOSBoot_SwitchPageMap4To5Func switchPageMap4To5 = (DexprOSBoot_SwitchPageMap4To5Func)funcStartAddress;

    // Prepare the scratch buffer that the function needs
    char* pScratchBuffer = (char*)pPageMapBuffer->pBuffer +
                           pPageMapBuffer->transitionData.codeScratchBufferOffset;
    memset(pScratchBuffer, 0, DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE);

    // Switch!
    switchPageMap4To5((uint64_t)pPML, (uint64_t)pScratchBuffer);

    return true;
}


static bool CreateAndSwitchPageMap4To5Above4G(void* pMemoryMapBuffer,
                                              UINTN memoryMapSize,
                                              UINTN memoryDescriptorSize,
                                              UINT32 memoryDescriptorVersion,
                                              EFI_PHYSICAL_ADDRESS framebufferBase,
                                              UINTN framebufferSize,
                                              const DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer)
{
    // Switch to a temporary 4-level page map to be 100% sure that we can
    // execute code in the buffer we have

    char* pPageMap4Memory = (char*)pPageMapBuffer->pBuffer +
                            pPageMapBuffer->transitionData.tmpPageMap4BufferOffset;

    void* pTmpPML = DexprOSBoot_SetupTransitionalPageMap4(pMemoryMapBuffer,
                                                      memoryMapSize,
                                                      memoryDescriptorSize,
                                                      memoryDescriptorVersion,
                                                      (void*)pPageMap4Memory,
                                                      pPageMapBuffer->transitionData.tmpPageMap4BufferSize);
    if (pTmpPML == NULL)
        return false;

    DexprOSBoot_SwitchPageMap((uint64_t)pTmpPML);


    // Create the 5-level page map

    char* pPageMap5Memory = (char*)pPageMapBuffer->pBuffer +
                            pPageMapBuffer->pageMapOffset;

    void* pPML = DexprOSBoot_SetupInitialPageMap5(pMemoryMapBuffer,
                                                memoryMapSize,
                                                memoryDescriptorSize,
                                                memoryDescriptorVersion,
                                                framebufferBase,
                                                framebufferSize,
                                                (void*)pPageMap5Memory,
                                                pPageMapBuffer->pageMapSize);
    if (pPML == NULL)
        return false;

    /*
    * Rellocate the memory region between DexprOSBoot_32BitPageMap5SwitchRegionStart
    * and DexprOSBoot_32BitPageMap5SwitchRegionEnd as it needs to be in
    * the 4 GiB of memory. The buffer we have is guaranteed to be in this
    * region thanks to the allocating function. We can safely just copy the
    * code as it's written in position independent assembly.
    */

    char* pPageSwitchRellocCode = (char*)pPageMapBuffer->pBuffer +
                                  pPageMapBuffer->transitionData.codeRellocBufferOffset;

    size_t codeRellocSize = (size_t)DexprOSBoot_32BitPageMap5SwitchRegionEnd -
                            (size_t)DexprOSBoot_32BitPageMap5SwitchRegionStart;

    memcpy(pPageSwitchRellocCode,
           DexprOSBoot_32BitPageMap5SwitchRegionStart,
           codeRellocSize);

    size_t pageSwitchReloadCode = (size_t)pPageSwitchRellocCode;
    

    DexprOSBoot_SwitchPageMap4To5Func switchPageMap4To5 = (DexprOSBoot_SwitchPageMap4To5Func)pageSwitchReloadCode;


    // Prepare the scratch buffer
    char* pScratchBuffer = (char*)pPageMapBuffer->pBuffer +
                           pPageMapBuffer->transitionData.codeScratchBufferOffset;
    memset(pScratchBuffer, 0, DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE);


    // Finally, switch the page map from the 4-level one to the 5-level one!
    switchPageMap4To5((uint64_t)pPML, (uint64_t)pScratchBuffer);


    return true;
}


static bool SwitchToIdentityPageMap(void* pMemoryMapBuffer,
                                    UINTN memoryMapSize,
                                    UINTN memoryDescriptorSize,
                                    UINT32 memoryDescriptorVersion,
                                    EFI_PHYSICAL_ADDRESS framebufferBase,
                                    UINTN framebufferSize,
                                    DexprOSBoot_PageMapSetupType pageMapSetupType,
                                    const DexprOSBoot_PageMapSetupBuffer* pPageMapBuffer)
{
    switch (pageMapSetupType)
    {
    case DEXPROSBOOT_PAGE_MAP_4_SETUP:
        return CreateAndSwitchPageMap4(pMemoryMapBuffer,
                                       memoryMapSize,
                                       memoryDescriptorSize,
                                       memoryDescriptorVersion,
                                       framebufferBase,
                                       framebufferSize,
                                       pPageMapBuffer);

    case DEXPROSBOOT_PAGE_MAP_5_SETUP:
        return CreateAndSwitchPageMap5(pMemoryMapBuffer,
                                       memoryMapSize,
                                       memoryDescriptorSize,
                                       memoryDescriptorVersion,
                                       framebufferBase,
                                       framebufferSize,
                                       pPageMapBuffer);

    case DEXPROSBOOT_PAGE_MAP_4_TO_5_TRANSITION:

        // Here we branch as some code may need to be rellocated or not
        // depending on the memory region it's placed in

        if ((size_t)DexprOSBoot_32BitPageMap5SwitchRegionEnd < 4294967296) // 4 GiB
        {
            return CreateAndSwitchPageMap4To5Below4G(pMemoryMapBuffer,
                                                     memoryMapSize,
                                                     memoryDescriptorSize,
                                                     memoryDescriptorVersion,
                                                     framebufferBase,
                                                     framebufferSize,
                                                     pPageMapBuffer);
        }
        else
        {
            return CreateAndSwitchPageMap4To5Above4G(pMemoryMapBuffer,
                                                     memoryMapSize,
                                                     memoryDescriptorSize,
                                                     memoryDescriptorVersion,
                                                     framebufferBase,
                                                     framebufferSize,
                                                     pPageMapBuffer);
        }
    }

    return false;
}


static DexprOSBoot_PageMapSetupType GetPageMapSetupType(void)
{
    if (DexprOSBoot_Has5LevelPagingSupport())
    {
        if (DexprOSBoot_Is5LevelPagingActive())
            return DEXPROSBOOT_PAGE_MAP_5_SETUP;
        else
            return DEXPROSBOOT_PAGE_MAP_4_TO_5_TRANSITION;
    }
    return DEXPROSBOOT_PAGE_MAP_4_SETUP;
}


EFI_STATUS DexprOSBoot_EndBootPhase(EFI_HANDLE imageHandle,
                                    EFI_SYSTEM_TABLE* pSystemTable,
                                    EFI_PHYSICAL_ADDRESS framebufferBase,
                                    UINTN framebufferSize,
                                    void** pOutMemoryMapBuffer,
                                    UINTN* pOutMemoryMapSize,
                                    UINTN* pOutMemoryMapKey,
                                    UINTN* pOutMemoryDescriptorSize,
                                    UINT32* pOutMemoryDescriptorVersion)
{
    EFI_STATUS status = 0;


    DexprOSBoot_PageMapSetupType pageMapSetupType = GetPageMapSetupType();

    DexprOSBoot_PageMapSetupBuffer pageMapBuffer;


    status = ExitBootServicesAndGetBuffer(imageHandle,
                                          pSystemTable,
                                          framebufferBase,
                                          framebufferSize,
                                          pageMapSetupType,
                                          pOutMemoryMapBuffer,
                                          pOutMemoryMapSize,
                                          pOutMemoryMapKey,
                                          pOutMemoryDescriptorSize,
                                          pOutMemoryDescriptorVersion,
                                          &pageMapBuffer);
    if (EFI_ERROR(status))
        return status;

    DexprOSBoot_DisableInterrupts();

    bool mapSuccess = SwitchToIdentityPageMap(*pOutMemoryMapBuffer,
                                              *pOutMemoryMapSize,
                                              *pOutMemoryDescriptorSize,
                                              *pOutMemoryDescriptorVersion,
                                              framebufferBase,
                                              framebufferSize,
                                              pageMapSetupType,
                                              &pageMapBuffer);
    return (mapSuccess ? EFI_SUCCESS : EFI_OUT_OF_RESOURCES);
}



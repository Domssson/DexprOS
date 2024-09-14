#ifndef DEXPROS_BOOT_X86_64_EFI_ENDBOOTPHASE_H_INCLUDED
#define DEXPROS_BOOT_X86_64_EFI_ENDBOOTPHASE_H_INCLUDED

#include <efi.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*
* Exits UEFI boot services and transfers full control to the OS.
* If a call to this function is successful, EFI memory map details are stored
* in output parameters, EFI boot services are no longer available and
* interrupts are disabled. The CPU uses a new page map that identity maps
* all ranges of usable (conventional) memory, the framebuffer and EFI services
* with all privilages. If the machine supports it, it setups a 5-level page
* map or a 4-level one otherwise.
*/
EFI_STATUS DexprOSBoot_EndBootPhase(EFI_HANDLE imageHandle,
                                    EFI_SYSTEM_TABLE* pSystemTable,
                                    EFI_PHYSICAL_ADDRESS framebufferBase,
                                    UINTN framebufferSize,
                                    void** pOutMemoryMapBuffer,
                                    UINTN* pOutMemoryMapSize,
                                    UINTN* pOutMemoryMapKey,
                                    UINTN* pOutMemoryDescriptorSize,
                                    UINT32* pOutMemoryDescriptorVersion);




#ifdef __cplusplus
}
#endif

#endif // DEXPROS_BOOT_X86_64_EFI_ENDBOOTPHASE_H_INCLUDED

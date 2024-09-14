#ifndef DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSETUP_H_INCLUDED
#define DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSETUP_H_INCLUDED

#include <efi.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


#define DEXPROSBOOT_INITIAL_PAGE_SIZE 4096


// Calculates size of a 4-level page map that identity maps each usable
// memory region, boot and runtime services, loader memory and the framebuffer
UINTN DexprOSBoot_CalculateInitialPageMap4Size(const void* pUefiMemoryMap,
                                               UINTN memoryMapSize,
                                               UINTN memoryDescriptorSize,
                                               UINTN memoryDescriptorVersion,
                                               EFI_PHYSICAL_ADDRESS framebufferBase,
                                               UINTN framebufferSize);


// Calculates size of a temporary 4-level page map that identity maps loader
// memory, boot and runtime services (stack may be stored there) with all
// privilages (read, write, execute). This kind of map acts as a transition
// map from 4-level to 5-level paging.
UINTN DexprOSBoot_CalculateTransitionalPageMap4Size(const void* pUefiMemoryMap,
                                                    UINTN memoryMapSize,
                                                    UINTN memoryDescriptorSize,
                                                    UINTN memoryDescriptorVersion);


// Calculates size of a 5-level page map that identity maps each usable
// memory region, boot and runtime services, loader memory and the framebuffer
UINTN DexprOSBoot_CalculateInitialPageMap5Size(const void* pUefiMemoryMap,
                                               UINTN memoryMapSize,
                                               UINTN memoryDescriptorSize,
                                               UINTN memoryDescriptorVersion,
                                               EFI_PHYSICAL_ADDRESS framebufferBase,
                                               UINTN framebufferSize);


// Creates a 4-level page map that identity maps each usable memory region,
// boot and runtime services, loader memory and the framebuffer.
//
// On success, returns a pointer to the PML4 table that can be moved to CR3.
// On failure, returns NULL.
void* DexprOSBoot_SetupInitialPageMap4(const void* pUefiMemoryMap,
                                       UINTN memoryMapSize,
                                       UINTN memoryDescriptorSize,
                                       UINTN memoryDescriptorVersion,
                                       EFI_PHYSICAL_ADDRESS framebufferBase,
                                       UINTN framebufferSize,
                                       void* pPageMapBuffer,
                                       UINTN pageMapBufferSize);


// Creates a temporary 4-level page map that identity maps loader memory,
// boot and runtime services (stack may be stored there) with all privilages
// (read, write, execute). This kind of map acts as a transition map from
// 4-level to 5-level paging.
// 
// On success, returns a pointer to the temporary PML4 table that can be
// moved to CR3.
// On failure, returns NULL.
void* DexprOSBoot_SetupTransitionalPageMap4(const void* pUefiMemoryMap,
                                            UINTN memoryMapSize,
                                            UINTN memoryDescriptorSize,
                                            UINTN memoryDescriptorVersion,
                                            void* pPageMapBuffer,
                                            UINTN pageMapBufferSize);


// Creates a 5-level page map that identity maps each usable memory region,
// boot and runtime services, loader memory and the framebuffer.
//
// On success, returns a pointer to the PML5 table that can be moved to CR3.
// On failure, returns NULL.
void* DexprOSBoot_SetupInitialPageMap5(const void* pUefiMemoryMap,
                                       UINTN memoryMapSize,
                                       UINTN memoryDescriptorSize,
                                       UINTN memoryDescriptorVersion,
                                       EFI_PHYSICAL_ADDRESS framebufferBase,
                                       UINTN framebufferSize,
                                       void* pPageMapBuffer,
                                       UINTN pageMapBufferSize);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSETUP_H_INCLUDED

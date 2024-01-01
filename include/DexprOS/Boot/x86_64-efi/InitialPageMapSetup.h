#ifndef DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSETUP_H_INCLUDED
#define DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSETUP_H_INCLUDED

#include <efi.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


UINTN DexprOSBoot_CalculatePageMap4SizeForLoader(const void* pUefiMemoryMap,
                                                 UINTN memoryMapSize,
                                                 UINTN memoryDescriptorSize,
                                                 UINTN memoryDescriptorVersion,
                                                 EFI_PHYSICAL_ADDRESS framebufferBase,
                                                 UINTN framebufferSize);

UINTN DexprOSBoot_CalculatePageMap5SizeForLoader(const void* pUefiMemoryMap,
                                                 UINTN memoryMapSize,
                                                 UINTN memoryDescriptorSize,
                                                 UINTN memoryDescriptorVersion,
                                                 EFI_PHYSICAL_ADDRESS framebufferBase,
                                                 UINTN framebufferSize);


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

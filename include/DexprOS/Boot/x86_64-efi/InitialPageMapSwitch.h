#ifndef DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSWITCH_H_INCLUDED
#define DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSWITCH_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


extern uint8_t DexprOSBoot_Is5LevelPagingActive(void);


// Used to switch page maps of the same number of levels
// (e.g. PML4 to PML4, PML5 to PML5).
extern void DexprOSBoot_SwitchPageMap(uint64_t pageMapBasePhysicalAddress);


// Used to switch from a 4-level page map to a 5-level one.
// The address must be in the first 4 GiBs in memory.
extern void DexprOSBoot_SwitchPageMap4LevelTo5Level(uint64_t pageMapBasePhysicalAddress);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSWITCH_H_INCLUDED

#ifndef DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSWITCH_H_INCLUDED
#define DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSWITCH_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


extern uint8_t DexprOSBoot_Has5LevelPagingSupport(void);

extern uint8_t DexprOSBoot_Is5LevelPagingActive(void);


extern void DexprOSBoot_DisableInterrupts(void);


// Used to switch page maps of the same number of levels
// (e.g. PML4 to PML4, PML5 to PML5).
extern void DexprOSBoot_SwitchPageMap(uint64_t pageMapBasePhysicalAddress);



/*
* Used to switch from a 4-level page map to a 5-level one.
* The address must be in the first 4 GiB in memory, as this function
* switches back to 32-bit protected mode in order to switch paging
* settings from 4-level to 5-level. The memory region from
* DexprOSBoot_32BitPageMap5SwitchRegionStart to
* DexprOSBoot_32BitPageMap5SwitchRegionEnd must be relocated below 
* the first 4 GiB of RAM before calling this function.
* The relocated function also needs 128 bytes of a scratch buffer
* of 16 byte alignment to write temporary data to.
*
* The format of the relocated function is like this:
* void SwitchPageMap4To5(uint64_t pageMapBasePhysicalAddress,
*                        uint64_t scratchBuffer);
*
* Both The address of the 5-level page map and the scratch buffer
* must also be below 4 GiB of RAM.
*
* The assembly code is written to be position independent.
*/
extern const char DexprOSBoot_32BitPageMap5SwitchRegionStart[];
extern const char DexprOSBoot_32BitPageMap5SwitchRegionEnd[];

#define DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_SIZE 128
#define DEXPROSBOOT_PAGE_MAP_4_TO_5_SCRATCH_BUFFER_ALIGN 16


typedef void (*DexprOSBoot_SwitchPageMap4To5Func)(uint64_t pageMapBasePhysicalAddress,
                                                  uint64_t scratchBuffer);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_BOOT_X86_64_EFI_INITIALPAGEMAPSWITCH_H_INCLUDED

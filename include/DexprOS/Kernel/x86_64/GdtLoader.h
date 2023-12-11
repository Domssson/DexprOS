#ifndef DEXPROS_KERNEL_X86_64_GDTLOADER_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_GDTLOADER_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


// The encoded GDT entries must be stored persistently in memory
// in order to work properly!
extern void DexprOS_LoadGDT(uint16_t locationSize,
                            uint64_t locationOffset,
                            uint16_t kernelCodeSegmentOffset,
                            uint64_t kernelDataSegmentOffset,
                            uint16_t taskStateSegmentOffset);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_GDTLOADER_H_INCLUDED

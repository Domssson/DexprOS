#ifndef DEXPROS_KERNEL_X86_64_GDTSETUP_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_GDTSETUP_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


// Setups the General Descriptor Table, loads it using lgdt,
// loads the first Task State Segment and flushes control registers.
// Beware that pOutTSSSegmentOffsets should point to an array of numTSSes elements!
void DexprOS_SetupGDT(const uint64_t* pTSSAddresses,
                      const uint16_t* pTSSSizes,
                      uint16_t numTSSes,
                      uint16_t* pOutKernelCodeSegmentOffset,
                      uint16_t* pOutUserBaseSegmentOffset,
                      uint16_t* pOutTSSSegmentOffsets);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_GDTSETUP_H_INCLUDED

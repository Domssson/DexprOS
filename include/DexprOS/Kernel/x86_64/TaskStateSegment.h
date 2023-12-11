#ifndef DEXPROS_KERNEL_X86_64_TASKSTATESEGMENT_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_TASKSTATESEGMENT_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_TaskStateSegment
{
    // Stack pointers loaded when changing the privilege lever to a higher one.
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;

    // Stack pointers loaded when firing an interrupt with IST value other than 0.
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;

    // Offset from the start of TSS to the I/O Permission Bit Map.
    uint16_t iopb;
} DexprOS_TaskStateSegment;


#define DEXPROS_X86_64_NUM_MAX_TSSes 1

void DexprOS_SetupTaskStateSegments(const DexprOS_TaskStateSegment* pTSSes,
                                    uint64_t* pOutLoadedTSSAddresses,
                                    uint16_t* pOutLoadedTSSSizes,
                                    uint16_t numTSSes);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_TASKSTATESEGMENT_H_INCLUDED

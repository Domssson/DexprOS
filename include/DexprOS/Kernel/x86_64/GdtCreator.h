#ifndef DEXPROS_KERNEL_X86_64_GDTCREATOR_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_GDTCREATOR_H_INCLUDED

#include "TaskStateSegment.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_GDTMemorySegmentDescriptor
{
    // The linear memory address of the start of the segment.
    uint32_t base;
    // The maximum addresable unit in bytes or pages depending
    // on the chosen granulity (20-bit).
    uint32_t limit;
    // Usage and type flags.
    uint8_t accessByte;
    uint8_t flags;
} DexprOS_GDTMemorySegmentDescriptor;


typedef struct DexprOS_GDTLongSystemSegmentDescriptor
{
    // The linear memory address of the start of the segment.
    uint64_t base;
    // The maximum addresable unit in bytes or pages depending
    // on the chosen granulity (20-bit).
    uint32_t limit;
    // Usage and type flags.
    uint8_t accessByte;
    uint8_t flags;
} DexprOS_GDTLongSystemSegmentDescriptor;


typedef enum DexprOS_GDTAccessBits
{
    // If this bit is not set on a memory segment, the CPU reading the segment's
    // memory will result in a page fault and then setting this bit.
    DEXPROS_GDT_ACCESS_ACCESSED_BIT = 0x1,
    // If set on data segments, makes them not only readable, but also
    // writable.
    DEXPROS_GDT_ACCESS_DATA_WRITABLE_BIT = 0x2,
    // If set on code segments, makes them not only executable, but also
    // readable. Code segments are never writable.
    DEXPROS_GDT_ACCESS_CODE_READABLE_BIT = 0x2,

    // If set on data segments, makes the limit go downwards and not upwards.
    DEXPROS_GDT_ACCESS_DATA_DIRECTION_DOWN_BIT = 0x4,
    // If set on code segments, makes the memory accesable from higher
    // privilaged modes (e.g. kernel code accessing apps' memory).
    DEXPROS_GDT_ACCESS_CODE_HIGHER_RING_CONFORMING_BIT = 0x4,

    // Determines whether a memory segment is a code segment and not a
    // data segment.
    DEXPROS_GDT_ACCESS_EXECUTABLE_BIT = 0x8,


    // Determines whether the segment is a Local Descriptor Table system
    // segment.
    DEXPROS_GDT_ACCESS_LDT_SEGMENT_BIT = 0x2,
    // Determines whether the segment is an avaible Task State Segment.
    DEXPROS_GDT_ACCESS_AVAIBLE_TASK_STATE_SEGMENT_BIT = 0x9,
    // Determines whether the segment is a busy Task State Segment.
    DEXPROS_GDT_ACCESS_BUSY_TASK_STATE_SEGMENT_BIT = 0xB,
    // Determines whether the segment is a memory segment and not a system one
    // (e.g. Task State Segment).
    DEXPROS_GDT_ACCESS_CODE_OR_DATA_SEGMENT_BIT = 0x10,

    
    // The segment can be addressed by user-space apps.
    DEXPROS_GDT_ACCESS_USERSPACE_BIT = 0x60,

    // Represents a valid segment.
    DEXPROS_GDT_ACCESS_PRESENT_BIT = 0x80
} DexprOS_GDTAccessBits;


typedef enum DexprOS_GDTFlagBits
{
    // Defines a 64-bit segment.
    DEXPROS_GDT_FLAG_LONG_MODE_BIT = 0x2,
    // Defines a 32-bit segment. If neither 64-bit nor 32-bit flags are set,
    // the segment is 16-bit.
    DEXPROS_GDT_FLAG_32BIT_BIT = 0x4,
    // If set, the segment size is the number of pages (4 KiB).
    // Otherwise, the segment size is the physical size of the segment.
    DEXPROS_GDT_FLAG_PAGE_GRANULITY_BIT = 0x8
} DexprOS_GDTFlagBits;


typedef struct DexprOS_GeneralDescriptorTable
{
    DexprOS_GDTMemorySegmentDescriptor nullDescriptor0;

    DexprOS_GDTMemorySegmentDescriptor kernelCodeSegment;
    DexprOS_GDTMemorySegmentDescriptor kernelDataSegment;

    DexprOS_GDTMemorySegmentDescriptor nullDescriptor1;
    
    DexprOS_GDTMemorySegmentDescriptor userDataSegment;
    DexprOS_GDTMemorySegmentDescriptor userCodeSegment;

    DexprOS_GDTLongSystemSegmentDescriptor taskStateSegments[DEXPROS_X86_64_NUM_MAX_TSSes];
    uint16_t numTSSes;

} DexprOS_GeneralDescriptorTable;


typedef struct DexprOS_GDTLocation
{
    uint16_t size;
    uint64_t offset;
} DexprOS_GDTLocation;



DexprOS_GeneralDescriptorTable DexprOS_CreateGDT(const uint64_t* pTSSAddresses,
                                                 const uint16_t* pTSSSizes,
                                                 uint16_t numTSSes);


uint64_t DexprOS_EncodeGDTMemorySegment(const DexprOS_GDTMemorySegmentDescriptor* pDesc);

void DexprOS_EncodeGDTSystemSegment(const DexprOS_GDTLongSystemSegmentDescriptor* pDesc,
                                    uint64_t* pOutEntries2);


#define DEXPROS_X86_64_MAX_GDT_ENTRIES (6 + DEXPROS_X86_64_NUM_MAX_TSSes * 2)


typedef uint64_t DexprOS_EncodedGDTEntries[DEXPROS_X86_64_MAX_GDT_ENTRIES];

// Beware that pOutTSSSegmentOffsets should point to an array of pTable->numTSSes elements!
void DexprOS_EncodeGDTTable(const DexprOS_GeneralDescriptorTable* pTable,
                            DexprOS_EncodedGDTEntries* pOutGdtEntries,
                            uint64_t* pOutNumUsedEntries,
                            uint16_t* pOutKernelCodeSegmentOffset,
                            uint64_t* pOutKernelDataSegmentOffset,
                            uint16_t* pOutUserBaseSegmentOffset,
                            uint16_t* pOutTaskStateSegmentOffsets);

// The encoded GDT entries must be stored persistently in memory
// in order to work properly!
DexprOS_GDTLocation DexprOS_FillGDTLocation(DexprOS_EncodedGDTEntries* pGdtEntries,
                                            uint64_t numGdtEntries);


#ifdef __cplusplus
}
#endif


#endif // DEXPROS_KERNEL_X86_64_GDTCREATOR_H_INCLUDED

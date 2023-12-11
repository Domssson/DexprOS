#include "DexprOS/Kernel/x86_64/GdtCreator.h"

DexprOS_GeneralDescriptorTable DexprOS_CreateGDT(const uint64_t* pTSSAddresses,
                                                 const uint16_t* pTSSSizes,
                                                 uint16_t numTSSes)
{
    DexprOS_GeneralDescriptorTable gdtTable = {
        .nullDescriptor0 = {
            .base = 0,
            .limit = 0,
            .accessByte = 0,
            .flags = 0
        },
        .kernelCodeSegment = {
            .base = 0,
            .limit = 0xFFFFF,
            .accessByte = DEXPROS_GDT_ACCESS_CODE_READABLE_BIT |
                          DEXPROS_GDT_ACCESS_EXECUTABLE_BIT |
                          DEXPROS_GDT_ACCESS_CODE_OR_DATA_SEGMENT_BIT |
                          DEXPROS_GDT_ACCESS_PRESENT_BIT,
            .flags = DEXPROS_GDT_FLAG_LONG_MODE_BIT |
                     DEXPROS_GDT_FLAG_PAGE_GRANULITY_BIT
        },
        .kernelDataSegment = {
            .base = 0,
            .limit = 0xFFFFF,
            .accessByte = DEXPROS_GDT_ACCESS_DATA_WRITABLE_BIT |
                          DEXPROS_GDT_ACCESS_CODE_OR_DATA_SEGMENT_BIT |
                          DEXPROS_GDT_ACCESS_PRESENT_BIT,
            .flags = DEXPROS_GDT_FLAG_32BIT_BIT |
                     DEXPROS_GDT_FLAG_PAGE_GRANULITY_BIT
        },
        .nullDescriptor1 = {
            .base = 0,
            .limit = 0,
            .accessByte = 0,
            .flags = 0
        },
        .userDataSegment = {
            .base = 0,
            .limit = 0xFFFFF,
            .accessByte = DEXPROS_GDT_ACCESS_DATA_WRITABLE_BIT |
                          DEXPROS_GDT_ACCESS_CODE_OR_DATA_SEGMENT_BIT |
                          DEXPROS_GDT_ACCESS_USERSPACE_BIT |
                          DEXPROS_GDT_ACCESS_PRESENT_BIT,
            .flags = DEXPROS_GDT_FLAG_32BIT_BIT |
                     DEXPROS_GDT_FLAG_PAGE_GRANULITY_BIT
        },
        .userCodeSegment = {
            .base = 0,
            .limit = 0xFFFFF,
            .accessByte = DEXPROS_GDT_ACCESS_CODE_READABLE_BIT |
                          DEXPROS_GDT_ACCESS_EXECUTABLE_BIT |
                          DEXPROS_GDT_ACCESS_CODE_OR_DATA_SEGMENT_BIT |
                          DEXPROS_GDT_ACCESS_USERSPACE_BIT |
                          DEXPROS_GDT_ACCESS_PRESENT_BIT,
            .flags = DEXPROS_GDT_FLAG_LONG_MODE_BIT |
                     DEXPROS_GDT_FLAG_PAGE_GRANULITY_BIT
        },
        .taskStateSegments = {{0}},
        .numTSSes = numTSSes
    };

    for (uint16_t i = 0; i < numTSSes && i < DEXPROS_X86_64_NUM_MAX_TSSes; ++i)
    {
        gdtTable.taskStateSegments[i].base = pTSSAddresses[i];
        gdtTable.taskStateSegments[i].limit = pTSSSizes[i];
        gdtTable.taskStateSegments[i].accessByte = DEXPROS_GDT_ACCESS_AVAIBLE_TASK_STATE_SEGMENT_BIT |
                                                   DEXPROS_GDT_ACCESS_PRESENT_BIT;
        gdtTable.taskStateSegments[i].flags = 0;
    }

    return gdtTable;
}


uint64_t DexprOS_EncodeGDTMemorySegment(const DexprOS_GDTMemorySegmentDescriptor* pDesc)
{
    uint64_t gdtEntry = 0;

    uint64_t base = (uint64_t)pDesc->base;
    uint64_t limit = (uint64_t)pDesc->limit;
    uint64_t accessByte = (uint64_t)pDesc->accessByte;
    uint64_t flags = (uint64_t)pDesc->flags;

    gdtEntry |= (limit & 0xFFFF);
    gdtEntry |= ((base & 0xFFFF) << 16);
    gdtEntry |= (((base >> 16) & 0xFF) << 32);
    gdtEntry |= (accessByte << 40);
    gdtEntry |= (((limit >> 16) & 0xF) << 48);
    gdtEntry |= ((flags & 0xF) << 52);
    gdtEntry |= (((base >> 24) & 0xFF) << 56);

    return gdtEntry;
}

void DexprOS_EncodeGDTSystemSegment(const DexprOS_GDTLongSystemSegmentDescriptor* pDesc,
                                    uint64_t* pOutEntries2)
{
    pOutEntries2[0] = 0;
    pOutEntries2[1] = 0;

    uint64_t base = pDesc->base;
    uint64_t limit = (uint64_t)pDesc->limit;
    uint64_t accessByte = (uint64_t)pDesc->accessByte;
    uint64_t flags = (uint64_t)pDesc->flags;

    pOutEntries2[0] |= (limit & 0xFFFF);
    pOutEntries2[0] |= ((base & 0xFFFF) << 16);
    pOutEntries2[0] |= (((base >> 16) & 0xFF) << 32);
    pOutEntries2[0] |= (accessByte << 40);
    pOutEntries2[0] |= (((limit >> 16) & 0xF) << 48);
    pOutEntries2[0] |= ((flags & 0xF) << 52);
    pOutEntries2[0] |= (((base >> 24) & 0xFF) << 56);

    pOutEntries2[1] |= ((base >> 32) & 0xFFFF);
}


void DexprOS_EncodeGDTTable(const DexprOS_GeneralDescriptorTable* pTable,
                            DexprOS_EncodedGDTEntries* pOutGdtEntries,
                            uint64_t* pOutNumUsedEntries,
                            uint16_t* pOutKernelCodeSegmentOffset,
                            uint64_t* pOutKernelDataSegmentOffset,
                            uint16_t* pOutUserBaseSegmentOffset,
                            uint16_t* pOutTaskStateSegmentOffsets)
{
    // Initialize everything to zeros
    for (uint64_t i = 0; i < DEXPROS_X86_64_MAX_GDT_ENTRIES; ++i)
    {
        (*pOutGdtEntries)[i] = 0;
    }


    (*pOutGdtEntries)[0] = DexprOS_EncodeGDTMemorySegment(&pTable->nullDescriptor0);
    (*pOutGdtEntries)[1] = DexprOS_EncodeGDTMemorySegment(&pTable->kernelCodeSegment);
    (*pOutGdtEntries)[2] = DexprOS_EncodeGDTMemorySegment(&pTable->kernelDataSegment);
    (*pOutGdtEntries)[3] = DexprOS_EncodeGDTMemorySegment(&pTable->nullDescriptor1);
    (*pOutGdtEntries)[4] = DexprOS_EncodeGDTMemorySegment(&pTable->userDataSegment);
    (*pOutGdtEntries)[5] = DexprOS_EncodeGDTMemorySegment(&pTable->userCodeSegment);

    for (uint64_t i = 0; i < pTable->numTSSes; ++i)
    {
        DexprOS_EncodeGDTSystemSegment(&pTable->taskStateSegments[i],
                                       &((*pOutGdtEntries)[6 + i * 2]));
    }


    *pOutNumUsedEntries = 6 + pTable->numTSSes * 2;
    *pOutKernelCodeSegmentOffset = sizeof(uint64_t);
    *pOutKernelDataSegmentOffset = sizeof(uint64_t) * 2;
    *pOutUserBaseSegmentOffset = sizeof(uint64_t) * 3;
    for (uint64_t i = 0; i < pTable->numTSSes; ++i)
    {
        pOutTaskStateSegmentOffsets[i] = sizeof(uint64_t) * (6 + i * 2);
    }
}

DexprOS_GDTLocation DexprOS_FillGDTLocation(DexprOS_EncodedGDTEntries* pGdtEntries,
                                            uint64_t numGdtEntries)
{
    DexprOS_GDTLocation gdtLocation = {
        .size = (sizeof(uint64_t) * numGdtEntries - 1),
        .offset = ((uint64_t)(&((*pGdtEntries)[0])))
    };
    return gdtLocation;
}



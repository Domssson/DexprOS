#include "DexprOS/Kernel/x86_64/GdtSetup.h"

#include "DexprOS/Kernel/x86_64/TaskStateSegment.h"
#include "DexprOS/Kernel/x86_64/GdtCreator.h"
#include "DexprOS/Kernel/x86_64/GdtLoader.h"
#include "DexprOS/DexprOSCommon.h"

#include <stdint.h>


DEXPROS_ALIGNAS(16)
static DexprOS_EncodedGDTEntries g_gdtEntriesStorage;


void DexprOS_SetupGDT(const uint64_t* pTSSAddresses,
                      const uint16_t* pTSSSizes,
                      uint16_t numTSSes,
                      uint16_t* pOutKernelCodeSegmentOffset,
                      uint16_t* pOutUserBaseSegmentOffset,
                      uint16_t* pOutTSSSegmentOffsets)
{
    DexprOS_GeneralDescriptorTable gdtTable = DexprOS_CreateGDT(pTSSAddresses,
                                                                pTSSSizes,
                                                                numTSSes);

    uint64_t numEntries = 0;
    uint16_t kernelCodeSegmentOffset = 0;
    uint64_t kernelDataSegmentOffset = 0;
    uint16_t userBaseSegmentOffset = 0;

    DexprOS_EncodeGDTTable(&gdtTable,
                           &g_gdtEntriesStorage,
                           &numEntries,
                           &kernelCodeSegmentOffset,
                           &kernelDataSegmentOffset,
                           &userBaseSegmentOffset,
                           pOutTSSSegmentOffsets);

    DexprOS_GDTLocation location = DexprOS_FillGDTLocation(&g_gdtEntriesStorage,
                                                           numEntries);

    DexprOS_LoadGDT(location.size,
                    location.offset,
                    kernelCodeSegmentOffset,
                    kernelDataSegmentOffset,
                    pOutTSSSegmentOffsets[0]);

    *pOutKernelCodeSegmentOffset = kernelCodeSegmentOffset;
    *pOutUserBaseSegmentOffset = userBaseSegmentOffset;
}


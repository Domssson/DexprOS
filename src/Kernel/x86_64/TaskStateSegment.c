#include "DexprOS/Kernel/x86_64/TaskStateSegment.h"

#include "DexprOS/DexprOSCommon.h"


DEXPROS_ALIGNAS(16)
static uint32_t g_DexprOS_TSSStorage[DEXPROS_X86_64_NUM_MAX_TSSes * 26];


void DexprOS_SetupTaskStateSegments(const DexprOS_TaskStateSegment* pTSSes,
                                    uint64_t* pOutLoadedTSSAddresses,
                                    uint16_t* pOutLoadedTSSSizes,
                                    uint16_t numTSSes)
{
    for (uint16_t i = 0; i < DEXPROS_X86_64_NUM_MAX_TSSes * 26; ++i)
        g_DexprOS_TSSStorage[i] = 0;
    

    for (uint16_t i = 0; i < numTSSes && i < DEXPROS_X86_64_NUM_MAX_TSSes; ++i)
    {
        const uint64_t rsp0 = pTSSes[i].rsp0;
        const uint64_t rsp1 = pTSSes[i].rsp1;
        const uint64_t rsp2 = pTSSes[i].rsp2;

        const uint64_t ist1 = pTSSes[i].ist1;
        const uint64_t ist2 = pTSSes[i].ist2;
        const uint64_t ist3 = pTSSes[i].ist3;
        const uint64_t ist4 = pTSSes[i].ist4;
        const uint64_t ist5 = pTSSes[i].ist5;
        const uint64_t ist6 = pTSSes[i].ist6;
        const uint64_t ist7 = pTSSes[i].ist7;

        const uint16_t iopb = pTSSes[i].iopb;


        g_DexprOS_TSSStorage[i * 26 + 1] = (uint32_t)(rsp0 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 2] = (uint32_t)((rsp0 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 3] = (uint32_t)(rsp1 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 4] = (uint32_t)((rsp1 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 5] = (uint32_t)(rsp2 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 6] = (uint32_t)((rsp2 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 9] = (uint32_t)(ist1 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 10] = (uint32_t)((ist1 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 11] = (uint32_t)(ist2 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 12] = (uint32_t)((ist2 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 13] = (uint32_t)(ist3 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 14] = (uint32_t)((ist3 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 15] = (uint32_t)(ist4 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 16] = (uint32_t)((ist4 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 17] = (uint32_t)(ist5 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 18] = (uint32_t)((ist5 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 19] = (uint32_t)(ist6 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 20] = (uint32_t)((ist6 >> 32) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 21] = (uint32_t)(ist7 & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 22] = (uint32_t)((ist7) & 0xFFFF);
        g_DexprOS_TSSStorage[i * 26 + 25] = (((uint32_t)iopb) << 16);

        *pOutLoadedTSSAddresses = (uint64_t)(&g_DexprOS_TSSStorage[i * 26]);
        *pOutLoadedTSSSizes = sizeof(uint32_t) * 26;
    }
}


#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREE_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREE_H_INCLUDED

#include "DexprOS/Kernel/Memory/PhysicalMemMap.h"

#include <stdint.h>
#include <stddef.h>


#define DEXPROS_PHYS_MEM_BLOCK_FULLY_AVAILABLE 0x1
#define DEXPROS_PHYS_MEM_BLOCK_HALF_AVAILABLE 0x2
#define DEXPROS_PHYS_MEM_BLOCK_QUARTER_AVAILABLE 0x4
// 1/8 of a block is available
#define DEXPROS_PHYS_MEM_BLOCK_1_8_AVAILABLE 0x8
#define DEXPROS_PHYS_MEM_BLOCK_1_16_AVAILABLE 0x10
#define DEXPROS_PHYS_MEM_BLOCK_1_32_AVAILABLE 0x20
#define DEXPROS_PHYS_MEM_BLOCK_1_64_AVAILABLE 0x40
#define DEXPROS_PHYS_MEM_BLOCK_1_128_OR_LESS_AVAILABLE 0x80


typedef struct DexprOS_PhysicalMemTreeLevel
{
    size_t numBlocks;

    union
    {
        uint8_t* pBlocks;
        uint8_t inPlaceBlocks[sizeof(uint8_t*)];
    } blocks;
    
} DexprOS_PhysicalMemTreeLevel;


typedef struct DexprOS_PhysicalMemTreeEntry
{
    DexprOS_PhysicalMemoryType type;
    DexprOS_PhysicalMemoryFlags flags;
    size_t numPages;

    DexprOS_PhysicalMemTreeLevel* pTreeLevels;
    size_t numTreeLevels;
} DexprOS_PhysicalMemTreeEntry;


#endif // DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREE_H_INCLUDED

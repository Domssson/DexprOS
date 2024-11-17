#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREE_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREE_H_INCLUDED

#include "DexprOS/Kernel/Memory/PhysicalMemMap.h"

#include <stdint.h>
#include <stddef.h>


// Blocks in the last level of the tree determine actual page availability
#define DEXPROS_PHYS_MEM_PAGE_AVAILABLE_BIT 0x80

#define DEXPROS_PHYS_MEM_PAGE_UNAVAILABLE 0x0

/*
* Blocks of previous levels determine availability of their consecutive levels.
* They don't map into pages and are internally called 'acceleration' layers.
*
* Each block splits into two in the next level, except for the last ones which
* may translate to just one block in next level if its blocks count is odd.
*
* The defines shall not be modified without modifying tree logic code
* as the code uses addition to quickly calculate values and doesn't
* reference these defines directly.
*/
#define DEXPROS_PHYS_MEM_BLOCK_FULLY_AVAILABLE_BIT 0x80
#define DEXPROS_PHYS_MEM_BLOCK_HALF_AVAILABLE_BIT 0x40
#define DEXPROS_PHYS_MEM_BLOCK_QUARTER_AVAILABLE_BIT 0x20
// 1/8 of a block is available
#define DEXPROS_PHYS_MEM_BLOCK_1_8_AVAILABLE_BIT 0x10
#define DEXPROS_PHYS_MEM_BLOCK_1_16_AVAILABLE_BIT 0x8
#define DEXPROS_PHYS_MEM_BLOCK_1_32_AVAILABLE_BIT 0x4
#define DEXPROS_PHYS_MEM_BLOCK_1_64_AVAILABLE_BIT 0x2
#define DEXPROS_PHYS_MEM_BLOCK_1_128_OR_LESS_AVAILABLE_BIT 0x1
#define DEXPROS_PHYS_MEM_BLOCK_UNAVAILABLE 0x0


typedef struct DexprOS_PhysicalMemTreeLevel
{
    DexprOS_PhysicalMemoryAddress startMappedAddress;
    size_t blockSize;
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
    DexprOS_PhysicalMemoryAddress startAddress;
    DexprOS_PhysicalMemoryAddress endAddress;
    size_t numPages;

    DexprOS_PhysicalMemTreeLevel* pTreeLevels;
    size_t numTreeLevels;


    struct DexprOS_PhysicalMemTreeEntry* pNextEntry;
} DexprOS_PhysicalMemTreeEntry;


typedef struct DexprOS_PhysicalMemTree
{
    DexprOS_PhysicalMemTreeEntry* pFirstEntry;
} DexprOS_PhysicalMemTree;


#endif // DEXPROS_KERNEL_MEMORY_PHYSICALMEMTREE_H_INCLUDED

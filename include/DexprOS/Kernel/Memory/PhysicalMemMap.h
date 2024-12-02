#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAP_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAP_H_INCLUDED

#include "MemoryDef.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_PhysicalMemoryRange
{
    DexprOS_PhysicalMemoryType memoryType;
    
    DexprOS_PhysicalMemoryAddress physicalAddress;
    DexprOS_PhysicalMemorySize rangeSize;

    DexprOS_PhysicalMemoryFlags flags;
} DexprOS_PhysicalMemoryRange;


typedef struct DexprOS_PhysicalMemMap
{
    size_t numEntries;
    DexprOS_PhysicalMemoryRange* pEntries;
} DexprOS_PhysicalMemMap;


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAP_H_INCLUDED

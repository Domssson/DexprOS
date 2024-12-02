#ifndef DEXPROS_KERNEL_MEMORY_INITIALMEMMAP_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_INITIALMEMMAP_H_INCLUDED

#include "MemoryDef.h"

#include <stddef.h>


typedef enum DexprOS_InitialMemMapMappedUsage
{
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_NONE,
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_MAPPED,
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_CODE,
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_KERNEL_DATA,
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_CODE,
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_BOOT_DATA,
    DEXPROS_INITIAL_MEMMAP_MAPPED_USAGE_INITIAL_MEMMAP
} DexprOS_InitialMemMapMappedUsage;


typedef struct DexprOS_InitialMemMapEntry
{
    DexprOS_PhysicalMemoryType memoryType;
    DexprOS_InitialMemMapMappedUsage usage;
    DexprOS_PhysicalMemoryFlags flags;

    DexprOS_PhysicalMemoryAddress physicalAddress;
    DexprOS_VirtualMemoryAddress virtualAddress;
    size_t numPhysicalPages;
} DexprOS_InitialMemMapEntry;


// The map's entries must be always sorted from the lowest physical address
// to the highest.

// An entry is available for allocation for any purpose if
// its type is USABLE and its usage is MAPPED.

typedef struct DexprOS_InitialMemMap
{
    DexprOS_InitialMemMapEntry* pEntries;
    size_t numEntries;
} DexprOS_InitialMemMap;


typedef void (*DexprOS_InitialMemMapPhysIterFunc)(DexprOS_PhysicalMemoryAddress physicalAddress,
                                                  DexprOS_PhysicalMemorySize rangeSize,
                                                  DexprOS_PhysicalMemoryType memType,
                                                  DexprOS_PhysicalMemoryFlags memFlags,
                                                  void* pUserData);


void DexprOS_IterateInitialMemMapCombinedPhysRanges(const DexprOS_InitialMemMap* pMemMap,
                                                    DexprOS_InitialMemMapPhysIterFunc pFunc,
                                                    void* pUserData);


#endif // DEXPROS_KERNEL_MEMORY_INITIALMEMMAP_H_INCLUDED

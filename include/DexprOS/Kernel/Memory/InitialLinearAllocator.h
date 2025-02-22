#ifndef DEXPROS_KERNEL_MEMORY_INITIALLINEARALLOCATOR_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_INITIALLINEARALLOCATOR_H_INCLUDED

#include "InitialMemMap.h"
#include "MemoryDef.h"

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_InitialLinearAllocator
{
    const DexprOS_InitialMemMap* pInitialMap;

    DexprOS_PhysicalMemoryAddress startAddress;

    size_t currentEntryIndex;
    size_t usedEntryPages;
} DexprOS_InitialLinearAllocator;


DexprOS_InitialLinearAllocator DexprOS_InitInitialLinearAllocator(const DexprOS_InitialMemMap* pInitialMap);


bool DexprOS_InitialLinearPagesAlloc(DexprOS_InitialLinearAllocator* pAllocator,
                                     size_t numPages,
                                     DexprOS_PhysicalMemoryAddress* pOutPhys,
                                     DexprOS_VirtualMemoryAddress* pOutVirt);


bool DexprOS_InitialLinearPageAlloc(DexprOS_InitialLinearAllocator* pAllocator,
                                    DexprOS_PhysicalMemoryAddress* pOutPhys,
                                    DexprOS_VirtualMemoryAddress* pOutVirt);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_MEMORY_INITIALLINEARALLOCATOR_H_INCLUDED

#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAPGEN_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAPGEN_H_INCLUDED

#include "PhysicalMemMap.h"
#include "InitialMemMap.h"

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


// Sets the required size and alignment of the buffer needed to create the new
// memory map
void DexprOS_GetPhysicalMemMapSize(const DexprOS_InitialMemMap* pInitialMap,
                                   size_t* pOutBufferSize,
                                   size_t* pOutBufferAlignment);


bool DexprOS_CreatePhysicalMemMap(DexprOS_PhysicalMemMap* pResult,
                                  const DexprOS_InitialMemMap* pInitialMap,
                                  void* pBuffer,
                                  size_t bufferSize);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAPGEN_H_INCLUDED

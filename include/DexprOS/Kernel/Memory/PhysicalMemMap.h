#ifndef DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAP_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_PHYSICALMEMMAP_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef size_t DexprOS_PhysicalMemoryAddress;
typedef size_t DexprOS_PhysicalMemorySize;
typedef uint64_t DexprOS_PhysicalMemoryFlags;


typedef enum DexprOS_PhysicalMemoryType
{
    DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE,
    DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED,
    DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED,
    DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE,
    DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE_PERSISTENT,
    DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_RECLAIM,
    DEXPROS_PHYSICAL_MEMORY_TYPE_ACPI_NVS,
    DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_CODE,
    DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_BOOT_SERVICES_DATA,
    DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_CODE,
    DEXPROS_PHYSICAL_MEMORY_TYPE_EFI_RUNTIME_SERVICES_DATA,

    DEXPROS_PHYSICAL_MEMORY_TYPE_Max
} DexprOS_PhysicalMemoryType;


// If this flag is set, the memory supports being configured as non-cachable
// using paging
#define DEXPROS_PHYSICAL_MEMORY_UC_BIT 0x1
#define DEXPROS_PHYSICAL_MEMORY_WRITE_COMBINE_BIT 0x2
#define DEXPROS_PHYSICAL_MEMORY_WRITE_THROUGH_BIT 0x4
#define DEXPROS_PHYSICAL_MEMORY_WRITE_BACK_BIT 0x8
#define DEXPROS_PHYSICAL_MEMORY_UCE_BIT 0x10
#define DEXPROS_PHYSICAL_MEMORY_WRITE_PROTECTED_BIT 0x20
#define DEXPROS_PHYSICAL_MEMORY_READ_PROTECTED_BIT 0x40
#define DEXPROS_PHYSICAL_MEMORY_EXECUTE_PROTECTED_BIT 0x80
#define DEXPROS_PHYSICAL_MEMORY_NON_VOLATILE_BIT 0x100
#define DEXPROS_PHYSICAL_MEMORY_MORE_RELIABLE_BIT 0x200
#define DEXPROS_PHYSICAL_MEMORY_READ_ONLY_BIT 0x400
#define DEXPROS_PHYSICAL_MEMORY_SPECIFIC_PURPOSE_BIT 0x800
#define DEXPROS_PHYSICAL_MEMORY_CPU_CRYPTO_BIT 0x1000
#define DEXPROS_PHYSICAL_MEMORY_EFI_RUNTIME_BIT 0x2000
#define DEXPROS_PHYSICAL_MEMORY_ISA_VALID_BIT 0x4000
#define DEXPROS_PHYSICAL_MEMORY_ISA_MASK_BITS 0xFFFF00000000000


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
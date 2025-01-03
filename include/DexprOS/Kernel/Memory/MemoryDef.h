#ifndef DEXPROS_KERNEL_MEMORY_MEMORYDEF_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_MEMORYDEF_H_INCLUDED

#include "Paging.h"

#include <stdint.h>
#include <stddef.h>


// Buffers allocated with this alignment can be used to store any object of
// standard alignment requirements (fundamental types, structs, unions, enums).
#define DEXPROS_FUNDAMENTAL_ALIGNMENT 16


// a must be power of 2
#define DEXPROS_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))


#define DEXPROS_ALIGN_FUNDAMENTAL(x) DEXPROS_ALIGN((x), DEXPROS_FUNDAMENTAL_ALIGNMENT)


typedef uint64_t DexprOS_VirtualMemoryAddress;


typedef uint64_t DexprOS_PhysicalMemoryAddress;
typedef size_t DexprOS_PhysicalMemorySize;
typedef uint64_t DexprOS_PhysicalMemoryFlags;


typedef enum DexprOS_PhysicalMemoryType
{
    DEXPROS_PHYSICAL_MEMORY_TYPE_UNUSABLE,
    DEXPROS_PHYSICAL_MEMORY_TYPE_FIRMWARE_RESERVED,
    DEXPROS_PHYSICAL_MEMORY_TYPE_UNACCEPTED,
    // On UEFI machines DEXPROS_PHYSICAL_MEMORY_TYPE_USABLE includes
    // EfiConventionalMemory, EfiLoaderCode and EfiLoaderData
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


// If this flag is set on a memory range, the memory region supports being
// configured as non-cachable using paging
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


#endif // DEXPROS_KERNEL_MEMORY_MEMORYDEF_H_INCLUDED

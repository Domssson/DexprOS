#ifndef DEXPROSBOOT_X86_64_ELF_ELFLOADER_H_INCLUDED
#define DEXPROSBOOT_X86_64_ELF_ELFLOADER_H_INCLUDED

#include "ElfHeaders.h"
#include "../FileSystems/BinaryStream.h"

#include <efi.h>


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


typedef enum DexprOSBoot_ElfSegmentType
{
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_NULL = 0,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_LOAD = 1,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_DYNAMIC = 2,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_INTERP = 3,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_NOTE = 4,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_SHLIB = 5,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_PHDR = 6,
    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_TLS = 7,

    DEXPROSBOOT_ELF_SEGMENT_TYPE_PROGRAM_UNKNOWN
} DexprOSBoot_ElfSegmentType;


typedef struct DexprOSBoot_LoadedElfSegment
{
    DexprOSBoot_ElfSegmentType segmentType;
    uint32_t flags;

    uint64_t physicalAddress;
    uint64_t preferredVirtualAddress;
    uint64_t numPages;
} DexprOSBoot_LoadedElfSegment;


typedef struct DexprOSBoot_LoadedElf
{
    DexprOSBoot_LoadedElfSegment* pSegments;
    size_t numSegments;

    uint64_t entryPointVirtAddress;
} DexprOSBoot_LoadedElf;



int DexprOSBoot_LoadElf64(DexprOSBoot_BinaryStream* pStream,
                          EFI_SYSTEM_TABLE* pSystemTable,
                          DexprOSBoot_LoadedElf* pOutLoadedElf);


void DexprOSBoot_FreeLoadedElfMemory(EFI_SYSTEM_TABLE* pSystemTable,
                                     DexprOSBoot_LoadedElf* pLoadedElf);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEXPROSBOOT_X86_64_ELF_ELFLOADER_H_INCLUDED

#ifndef DEXPROSBOOT_X86_64_ELF_ELFLOADER_H_INCLUDED
#define DEXPROSBOOT_X86_64_ELF_ELFLOADER_H_INCLUDED

#include "ElfHeaders.h"
#include "../FileSystems/BinaryStream.h"

#include <efi.h>


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



int DexprOSBoot_LoadElf64(DexprOSBoot_BinaryStream* pStream,
                          EFI_SYSTEM_TABLE* pSystemTable);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEXPROSBOOT_X86_64_ELF_ELFLOADER_H_INCLUDED

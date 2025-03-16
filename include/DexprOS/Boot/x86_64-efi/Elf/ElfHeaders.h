#ifndef DEXPROSBOOT_X86_64_EFI_ELF_ELFHEADERS_H_INCLUDED
#define DEXPROSBOOT_X86_64_EFI_ELF_ELFHEADERS_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


#define DEXPROSBOOT_ELF_IDENT_SIZE 16
#define DEXPROSBOOT_ELF_EI_MAG 0
#define DEXPROSBOOT_ELF_EI_CLASS 4
#define DEXPROSBOOT_ELF_EI_DATA 5
#define DEXPROSBOOT_ELF_EI_VERSION 6
#define DEXPROSBOOT_ELF_EI_OSABI 7


#define DEXPROSBOOT_ELF_MAGIC {0x7F, 'E', 'L', 'F'}
#define DEXPROSBOOT_ELF_CLASS_32_BIT 1
#define DEXPROSBOOT_ELF_CLASS_64_BIT 2
#define DEXPROSBOOT_ELF_DATA_LE 1 // Little endian
#define DEXPROSBOOT_ELF_DATA_BE 2 // Big endian

#define DEXPROSBOOT_ELF_EI_VERSION_CURRENT 1

#define DEXPROSBOOT_ELF_ABI_SYSTEMV 0
#define DEXPROSBOOT_ELF_REQ_ABI DEXPROSBOOT_ELF_ABI_SYSTEMV


#define DEXPROSBOOT_ELF_ET_EXEC 2

#define DEXPROSBOOT_ELF_EM_X86_64 0x3E

#define DEXPROSBOOT_ELF_EV_CURRENT 1

#define DEXPROSBOOT_ELF_ELFHEADER64_MIN_SIZE 64


#define DEXPROSBOOT_ELF_PROGRAMHEADER64_MIN_SIZE 56

#define DEXPROSBOOT_ELF_PT_NULL 0
#define DEXPROSBOOT_ELF_PT_LOAD 1
#define DEXPROSBOOT_ELF_PT_DYNAMIC 2
#define DEXPROSBOOT_ELF_PT_INTERP 3
#define DEXPROSBOOT_ELF_PT_NOTE 4
#define DEXPROSBOOT_ELF_PT_SHLIB 5
#define DEXPROSBOOT_ELF_PT_PHDR 6
#define DEXPROSBOOT_ELF_PT_TLS 7

#define DEXPROSBOOT_ELF_PF_X 1
#define DEXPROSBOOT_ELF_PF_W 2
#define DEXPROSBOOT_ELF_PF_R 4



typedef struct DexprOSBoot_ElfHeader64
{
    unsigned char e_ident[DEXPROSBOOT_ELF_IDENT_SIZE];

    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;

    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;

    uint32_t e_flags;

    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} DexprOSBoot_ElfHeader64;


typedef struct DexprOSBoot_ElfProgramHeader64
{
    uint32_t p_type;
    uint32_t p_flags;

    uint64_t p_offset;

    uint64_t p_vaddr;
    uint64_t p_paddr;

    uint64_t p_filesz;
    uint64_t p_memsz;

    uint64_t p_align;
} DexprOSBoot_ElfProgramHeader64;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEXPROSBOOT_X86_64_EFI_ELF_ELFHEADERS_H_INCLUDED

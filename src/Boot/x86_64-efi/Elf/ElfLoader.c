#include "DexprOS/Boot/x86_64-efi/Elf/ElfLoader.h"

#include "DexprOS/Kernel/kstdlib/string.h"

#include <stdbool.h>

#include <efi.h>


EFI_STATUS print(const char* text);

EFI_STATUS printHex(uint64_t number);

EFI_STATUS printDec(uint64_t number);



static bool VerifyIdentElf64LE(DexprOSBoot_ElfHeader64* pHeader)
{
    unsigned char* pIdent = pHeader->e_ident;

    unsigned char magic[4] = DEXPROSBOOT_ELF_MAGIC;

    if (memcmp(&pIdent[DEXPROSBOOT_ELF_EI_MAG], magic, sizeof(magic)) != 0)
        return false;

    if (pIdent[DEXPROSBOOT_ELF_EI_CLASS] != DEXPROSBOOT_ELF_CLASS_64_BIT)
        return false;

    // Little endian
    if (pIdent[DEXPROSBOOT_ELF_EI_DATA] != DEXPROSBOOT_ELF_DATA_LE)
        return false;

    if (pIdent[DEXPROSBOOT_ELF_EI_VERSION] != DEXPROSBOOT_ELF_EI_VERSION_CURRENT)
        return false;

    // System V ABI
    if (pIdent[DEXPROSBOOT_ELF_EI_OSABI] != DEXPROSBOOT_ELF_REQ_ABI)
        return false;

    return true;
}


static bool LoadElfIdent64LE(DexprOSBoot_ElfHeader64* pOut, DexprOSBoot_BinaryStream* pStream)
{
    if (pStream->read(pStream, pOut->e_ident, sizeof(pOut->e_ident), 1) != 1)
        return false;

    return VerifyIdentElf64LE(pOut);
}


static bool VerifyElfHeader64(DexprOSBoot_ElfHeader64* pHeader)
{
    if (pHeader->e_type != DEXPROSBOOT_ELF_ET_EXEC)
        return false;

    if (pHeader->e_machine != DEXPROSBOOT_ELF_EM_X86_64)
        return false;

    if (pHeader->e_version != DEXPROSBOOT_ELF_EV_CURRENT)
        return false;

    if (pHeader->e_ehsize < DEXPROSBOOT_ELF_ELFHEADER64_MIN_SIZE)
        return false;

    if (pHeader->e_phentsize < DEXPROSBOOT_ELF_PROGRAMHEADER64_MIN_SIZE)
        return false;

    return true;
}

static bool LoadElfHeader64(DexprOSBoot_ElfHeader64* pOut, DexprOSBoot_BinaryStream* pStream)
{
    if (pStream->read(pStream, &pOut->e_type, sizeof(pOut->e_type), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_machine, sizeof(pOut->e_machine), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_version, sizeof(pOut->e_version), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_entry, sizeof(pOut->e_entry), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_phoff, sizeof(pOut->e_phoff), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_shoff, sizeof(pOut->e_shoff), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_flags, sizeof(pOut->e_flags), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_ehsize, sizeof(pOut->e_ehsize), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_phentsize, sizeof(pOut->e_phentsize), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_phnum, sizeof(pOut->e_phnum), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_shentsize, sizeof(pOut->e_shentsize), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_shnum, sizeof(pOut->e_shnum), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->e_shstrndx, sizeof(pOut->e_shstrndx), 1) != 1)
        return false;

    return VerifyElfHeader64(pOut);
}


static bool VerifyElfProgramHeader64(DexprOSBoot_ElfProgramHeader64* pHeader)
{
    // Verify alignment

    if (pHeader->p_align == 0 || pHeader->p_align == 1)
        return true;

    for (uint64_t i = 1; i < 64; i++)
    {
        uint64_t powerOf2 = UINT64_C(1) << i;
        if (pHeader->p_align == powerOf2)
        {
            return (pHeader->p_vaddr % pHeader->p_align) == (pHeader->p_offset % pHeader->p_align);
        }
    }

    return false;
}

bool LoadElfProgramHeader64(DexprOSBoot_ElfProgramHeader64* pOut,
                            uint64_t offset,
                            DexprOSBoot_BinaryStream* pStream)
{
    pStream->setStreamPos(pStream, offset);

    if (pStream->read(pStream, &pOut->p_type, sizeof(pOut->p_type), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_flags, sizeof(pOut->p_flags), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_offset, sizeof(pOut->p_offset), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_vaddr, sizeof(pOut->p_vaddr), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_paddr, sizeof(pOut->p_paddr), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_filesz, sizeof(pOut->p_filesz), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_memsz, sizeof(pOut->p_memsz), 1) != 1)
        return false;
    if (pStream->read(pStream, &pOut->p_align, sizeof(pOut->p_align), 1) != 1)
        return false;

    return VerifyElfProgramHeader64(pOut);
}


bool MapElfProgramHeader64(DexprOSBoot_ElfProgramHeader64* pHeader)
{
    print("ELF program header:\n");
    print("Type: ");
    printHex(pHeader->p_type);
    print("\nFlags: ");
    printHex(pHeader->p_flags);
    print("\n\n");

    return true;
}


int DexprOSBoot_LoadElf64(DexprOSBoot_BinaryStream* pStream)
{
    DexprOSBoot_ElfHeader64 elfHeader;

    if (!LoadElfIdent64LE(&elfHeader, pStream))
        return 1;

    if (!LoadElfHeader64(&elfHeader, pStream))
        return 2;

    for (unsigned i = 0; i < elfHeader.e_phnum; ++i)
    {
        DexprOSBoot_ElfProgramHeader64 programHeader;

        uint64_t offset = elfHeader.e_phoff + i * elfHeader.e_phentsize;

        if (!LoadElfProgramHeader64(&programHeader,
                                    offset,
                                    pStream))
            return 3;

        if (!MapElfProgramHeader64(&programHeader))
            return 4;
    }

    return 0;
}


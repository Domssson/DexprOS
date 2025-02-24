#include "DexprOS/Boot/x86_64-efi/EmbeddedKernel.h"
#include "DexprOS/Kernel/kstdlib/string.h"

#include <efi.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


static EFI_STATUS print(EFI_SYSTEM_TABLE* pSystemTable, const char* text);

static EFI_STATUS printHex(EFI_SYSTEM_TABLE* pSystemTable, uint64_t number);

static EFI_STATUS printDec(EFI_SYSTEM_TABLE* pSystemTable, uint64_t number);


EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* pSystemTable)
{
    EFI_STATUS status = 0;


    pSystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    if (EFI_ERROR(status))
        return status;


    status = pSystemTable->ConOut->ClearScreen(pSystemTable->ConOut);
    if (EFI_ERROR(status))
        return status;


    status = pSystemTable->ConOut->EnableCursor(pSystemTable->ConOut, TRUE);
    if (EFI_ERROR(status))
        return status;


    status = print(pSystemTable, "Hello world!\n");
    if (EFI_ERROR(status))
        return status;

    status = print(pSystemTable, "Path to the kernel that was embedded: ");
    if (EFI_ERROR(status))
        return status;

    status = print(pSystemTable, DEXPROSBOOT_EMBED_KERNEL_PATH);
    if (EFI_ERROR(status))
        return status;

    status = print(pSystemTable, "\nKernel size: ");
    if (EFI_ERROR(status))
        return status;


    uint64_t kernelSize = DexprOSBoot_KernelExecEnd - DexprOSBoot_KernelExecStart;

    status = printDec(pSystemTable, kernelSize);
    if (EFI_ERROR(status))
        return status;

    status = print(pSystemTable, "\nKernel mem start: ");
    if (EFI_ERROR(status))
        return status;

    status = printHex(pSystemTable, (uint64_t)DexprOSBoot_KernelExecStart);
    if (EFI_ERROR(status))
        return status;

    status = print(pSystemTable, "\nKernel mem end: ");
    if (EFI_ERROR(status))
        return status;

    status = printHex(pSystemTable, (uint64_t)DexprOSBoot_KernelExecEnd);
    if (EFI_ERROR(status))
        return status;

    status = print(pSystemTable, "\n");
    if (EFI_ERROR(status))
        return status;


    status = pSystemTable->ConIn->Reset(pSystemTable->ConIn, FALSE);
    if (EFI_ERROR(status))
        return status;

    EFI_INPUT_KEY key;
    while ((status = pSystemTable->ConIn->ReadKeyStroke(pSystemTable->ConIn, &key)) == EFI_NOT_READY);

    (void)imageHandle;

    return status;
}


EFI_STATUS print(EFI_SYSTEM_TABLE* pSystemTable, const char* text)
{
    EFI_STATUS status = EFI_SUCCESS;

    while (text[0] != '\0')
    {
        if (text[0] == '\n')
            status = pSystemTable->ConOut->OutputString(pSystemTable->ConOut, L"\r\n");
        else
        {
            CHAR16 efiString[2] = {text[0], '\0'};
            status = pSystemTable->ConOut->OutputString(pSystemTable->ConOut, efiString);
        }

        if (EFI_ERROR(status))
            return status;

        text += 1;
    }

    return status;
}


EFI_STATUS printHex(EFI_SYSTEM_TABLE* pSystemTable, uint64_t number)
{
    CHAR16 strings[16][2] = {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"A", L"B", L"C", L"D", L"E", L"F"};

    EFI_STATUS status = EFI_SUCCESS;

    uint64_t power = 15;

    status = pSystemTable->ConOut->OutputString(pSystemTable->ConOut, L"0x");
    if (EFI_ERROR(status))
        return status;


    while ((number >> (4 * power)) == 0 && power > 0)
        power -= 1;


    while (power > 0)
    {
        unsigned index = (number >> (4 * power)) & 0xF;

        status = pSystemTable->ConOut->OutputString(pSystemTable->ConOut, strings[index]);
        if (EFI_ERROR(status))
            return status;

        power -= 1;
    }


    unsigned index0 = number & 0xF;
    status = pSystemTable->ConOut->OutputString(pSystemTable->ConOut, strings[index0]);


    return status;
}


EFI_STATUS printDec(EFI_SYSTEM_TABLE* pSystemTable, uint64_t number)
{
    CHAR16 strings[10][2] = {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9"};

    EFI_STATUS status = EFI_SUCCESS;

    uint64_t checkNumber = number;
    uint64_t denom = 0x8AC7230489E80000;


    while ((number / denom) == 0 && denom > 1)
        denom /= 10;


    while (denom > 0)
    {
        unsigned index = checkNumber / denom;

        status = pSystemTable->ConOut->OutputString(pSystemTable->ConOut, strings[index]);
        if (EFI_ERROR(status))
            return status;

        checkNumber = checkNumber % denom;
        denom /= 10;
    }

    return status;
}



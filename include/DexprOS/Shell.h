#ifndef DEXPROS_SHELL_H_INCLUDED
#define DEXPROS_SHELL_H_INCLUDED

#include "DexprOS/Drivers/Graphics/GraphicsDriver.h"
#include "DexprOS/Kernel/KeyboardInput.h"
#include "DexprOS/DexprOSCommon.h"

#include <stdint.h>
#include <stdbool.h>

#include <efi.h>

#ifdef __cplusplus
extern "C"
{
#endif


#define DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS 65536 // 512 * 128
#define DEXPROS_SHELL_NUM_MAX_COMMAND_CHARACTERS 256


typedef DexprOS_UnicodeCodepoint DexprOS_Shell_Char;


typedef struct DexprOS_Shell
{
    DexprOS_Shell_Char onScreenCharacters[DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS];

    DexprOS_Shell_Char displayCharacters[DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS];
    unsigned displayRingBufferStart;
    unsigned displayRingBufferEnd;

    unsigned firstDisplayChar;

    int outputCursorX;
    int outputCursorY;

    DexprOS_Shell_Char commandCharacters[DEXPROS_SHELL_NUM_MAX_COMMAND_CHARACTERS];
    unsigned numCommandCharacters;
    bool commandPromptEnabled;


    unsigned currentLine;


    unsigned consoleWidth;
    unsigned consoleHeight;

    unsigned numLines;

    const char* promptText;

    DexprOS_GraphicsDriver* pGraphicsDrv;
    DexprOS_GrGraphicsColor backgroundColor;

    EFI_SYSTEM_TABLE* pUefiSystemTable;
} DexprOS_Shell;



void DexprOS_CreateShell(DexprOS_Shell* pShell,
                         EFI_SYSTEM_TABLE* pUefiSystemTable,
                         DexprOS_GraphicsDriver* pGraphicsDrv,
                         unsigned consoleWidth,
                         unsigned consoleHeight);

void DexprOS_ShellPutChar(DexprOS_Shell* pShell, DexprOS_Shell_Char character);

void DexprOS_ShellPuts(DexprOS_Shell* pShell, const char* msg);

void DexprOS_ShellPutsUTF32(DexprOS_Shell* pShell, const DexprOS_Shell_Char* msg);

void DexprOS_ShellFlushStdOut(DexprOS_Shell* pShell);

void DexprOS_ShellActivatePrompt(DexprOS_Shell* pShell);


void DexprOS_ShellKeyboardCharacterEvent(DexprOS_Shell* pShell,
                                         DexprOS_UnicodeCodepoint codepoint);

void DexprOS_ShellKeyboardKeyEvent(DexprOS_Shell* pShell,
                                   DexprOS_KeyID keyID,
                                   DexprOS_KeyState state);



void DexprOS_DestroyShell(DexprOS_Shell* pShell);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_SHELL_H_INCLUDED

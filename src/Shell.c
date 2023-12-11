#include "DexprOS/Shell.h"

#include "DexprOS/Kernel/BuiltInBitmapFont.h"

#include <stddef.h>


static void DexprOS_ShellEcho(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength);
static void DexprOS_ShellClear(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength);
static void DexprOS_ShellHelp(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength);
static void DexprOS_ShellShutdown(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength);
static void DexprOS_ShellReboot(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength);

typedef struct DexprOS_Shell_CommandFunc
{
    const char* commandText;
    void (*commandFunc)(DexprOS_Shell*, const DexprOS_Shell_Char*, size_t);
} DexprOS_Shell_CommandFunc;

#define DEXPROS_SHELL_NUM_BUILTIN_COMMANDS 5
static const DexprOS_Shell_CommandFunc g_DexprOS_ShellBuiltInCommands[DEXPROS_SHELL_NUM_BUILTIN_COMMANDS] = {
    {"echo", DexprOS_ShellEcho},
    {"clear", DexprOS_ShellClear},
    {"help", DexprOS_ShellHelp},
    {"shutdown", DexprOS_ShellShutdown},
    {"reboot", DexprOS_ShellReboot}
};


void DexprOS_CreateShell(DexprOS_Shell* pShell,
                         EFI_SYSTEM_TABLE* pUefiSystemTable,
                         DexprOS_GraphicsDriver* pGraphicsDrv,
                         unsigned consoleWidth,
                         unsigned consoleHeight)
{
    for (size_t i = 0; i < DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS; ++i)
        pShell->onScreenCharacters[i] = 0;

    for (size_t i = 0; i < DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS; ++i)
        pShell->displayCharacters[i] = 0;
    pShell->displayRingBufferStart = 0;
    pShell->displayRingBufferEnd = 0;

    pShell->firstDisplayChar = 0;

    pShell->outputCursorX = 0;
    pShell->outputCursorY = 0;

    for (size_t i = 0; i < DEXPROS_SHELL_NUM_MAX_COMMAND_CHARACTERS; ++i)
        pShell->commandCharacters[i] = 0;
    pShell->numCommandCharacters = 0;
    pShell->commandPromptEnabled = false;

    pShell->currentLine = 0;

    pShell->consoleWidth = consoleWidth;
    pShell->consoleHeight = consoleHeight;
    pShell->numLines = consoleHeight / DexprOS_BuiltInBitmapFontGetLineHeight();
    pShell->numLines = (pShell->numLines > 1 ? (pShell->numLines - 1) : 1);

    pShell->promptText = "DexprOS # ";

    pShell->pGraphicsDrv = pGraphicsDrv;
    pShell->backgroundColor.r = 0;
    pShell->backgroundColor.g = 128;
    pShell->backgroundColor.b = 128;

    pShell->pUefiSystemTable = pUefiSystemTable;


    pGraphicsDrv->clearScreen(pGraphicsDrv->pDriverData, pShell->backgroundColor);
    pGraphicsDrv->swapBuffers(pGraphicsDrv->pDriverData);
}

void DexprOS_DestroyShell(DexprOS_Shell* pShell)
{
    (void)pShell;
}


void DexprOS_ShellPutChar(DexprOS_Shell* pShell, DexprOS_Shell_Char character)
{
    if (character == 0)
        return;

    unsigned bufferStart = pShell->displayRingBufferStart;
    unsigned currentChar = pShell->displayRingBufferEnd;


    pShell->displayCharacters[currentChar] = character;
    currentChar += 1;
    currentChar %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
    
    if (currentChar == bufferStart)
    {
        bufferStart += 1;
        bufferStart %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
    }
    if (currentChar == pShell->firstDisplayChar)
    {
        pShell->firstDisplayChar += 1;
        pShell->firstDisplayChar %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
    }


    pShell->displayRingBufferStart = bufferStart;
    pShell->displayRingBufferEnd = currentChar;
}

void DexprOS_ShellPuts(DexprOS_Shell* pShell, const char* msg)
{
    if (msg == NULL)
        return;


    unsigned bufferStart = pShell->displayRingBufferStart;
    unsigned currentChar = pShell->displayRingBufferEnd;

    size_t msgLength = 0;
    for (; msg[msgLength] != '\0'; ++msgLength)
    {
        DexprOS_Shell_Char character = (DexprOS_Shell_Char)msg[msgLength];
        
        pShell->displayCharacters[currentChar] = character;
        currentChar += 1;
        currentChar %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
        
        if (currentChar == bufferStart)
        {
            bufferStart += 1;
            bufferStart %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
        }
        if (currentChar == pShell->firstDisplayChar)
        {
            pShell->firstDisplayChar += 1;
            pShell->firstDisplayChar %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
        }
    }

    pShell->displayRingBufferStart = bufferStart;
    pShell->displayRingBufferEnd = currentChar;
}

void DexprOS_ShellPutsUTF32(DexprOS_Shell* pShell, const DexprOS_Shell_Char* msg)
{
    if (msg == NULL)
        return;


    unsigned bufferStart = pShell->displayRingBufferStart;
    unsigned currentChar = pShell->displayRingBufferEnd;

    size_t msgLength = 0;
    for (; msg[msgLength] != 0; ++msgLength)
    {
        DexprOS_Shell_Char character = msg[msgLength];
        
        pShell->displayCharacters[currentChar] = character;
        currentChar += 1;
        currentChar %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
        
        if (currentChar == bufferStart)
        {
            bufferStart += 1;
            bufferStart %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
        }
        if (currentChar == pShell->firstDisplayChar)
        {
            pShell->firstDisplayChar += 1;
            pShell->firstDisplayChar %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS;
        }
    }

    pShell->displayRingBufferStart = bufferStart;
    pShell->displayRingBufferEnd = currentChar;
}


static void DexprOS_ShellDeleteLastDisplayChar(DexprOS_Shell* pShell)
{
    if (pShell->displayRingBufferStart == pShell->displayRingBufferEnd)
        return;

    if (pShell->displayRingBufferEnd == 0)
        pShell->displayRingBufferEnd = DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS - 1;
    else
        pShell->displayRingBufferEnd -= 1;
}

static void DexprOS_ShellResetText(DexprOS_Shell* pShell)
{
    const unsigned lineHeight = DexprOS_BuiltInBitmapFontGetLineHeight();
    
    int cursorX = 0;
    int cursorY = 0;

    for (size_t i = 0; i < DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS; ++i)
    {
        DexprOS_Shell_Char character = pShell->onScreenCharacters[i];

        if (character == 0) // Null terminator character
            break;
        
        if (character == 10) // New line
        {
            cursorX = 0;
            cursorY += lineHeight;
            continue;
        }

        DexprOS_BuiltInBitmapFontMetrics metrics =
        DexprOS_BuiltInBitmapFontGetGlyphMetrics(character);

        if (cursorX + metrics.advanceX > (int)pShell->consoleWidth)
        {
            cursorX = 0;
            cursorY += lineHeight;
        }

        DexprOS_GrImageRegion clearRegion = {
            .x = cursorX + metrics.offsetX,
            .y = cursorY + metrics.offsetY,
            .width = metrics.width,
            .height = metrics.height
        };
        pShell->pGraphicsDrv->clearScreenRegions(pShell->pGraphicsDrv->pDriverData,
                                                 1,
                                                 &clearRegion,
                                                 pShell->backgroundColor);

        cursorX += metrics.advanceX;
    }

    for (size_t i = 0; i < DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS; ++i)
        pShell->onScreenCharacters[i] = 0;
}

static unsigned DexprOS_ShellGetNumOverflowLines(const DexprOS_Shell* pShell)
{
    unsigned numLines = 0;

    int cursorX = 0;

    for (unsigned i = pShell->firstDisplayChar; i != pShell->displayRingBufferEnd; ++i, i %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS)
    {
        DexprOS_Shell_Char character = pShell->displayCharacters[i];


        int advanceX = DexprOS_BuiltInBitmapFontGetGlyphAdvanceX(character);

        if (character == 10) // New line character
        {
            cursorX = 0;
            numLines += 1;
            continue;
        }
        if (cursorX + advanceX > (int)pShell->consoleWidth)
        {
            cursorX = 0;
            numLines += 1;
        }

        cursorX += advanceX;
    }

    if (numLines >= pShell->numLines)
        return numLines - pShell->numLines + 1;
    return 0;
}

static void DexprOS_ShellScrollDisplay(DexprOS_Shell* pShell, unsigned numLines)
{
    unsigned numPlacedLines = 0;

    int cursorX = 0;

    unsigned i = pShell->firstDisplayChar;
    for (; i != pShell->displayRingBufferEnd; ++i, i %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS)
    {
        if (numPlacedLines == numLines)
            break;

        DexprOS_Shell_Char character = pShell->displayCharacters[i];


        if (character == 10) // New line character
        {
            cursorX = 0;
            numPlacedLines += 1;
            continue;
        }

        int advanceX = DexprOS_BuiltInBitmapFontGetGlyphAdvanceX(character);

        if (cursorX + advanceX > (int)pShell->consoleWidth)
        {
            cursorX = 0;
            numPlacedLines += 1;
        }

        cursorX += advanceX;
    }
    pShell->firstDisplayChar = i;
}

static void DexprOS_ShellSetText(DexprOS_Shell* pShell, unsigned firstChar, unsigned endChar)
{
    const unsigned lineHeight = DexprOS_BuiltInBitmapFontGetLineHeight();

    size_t onScreenIndex = 0;

    pShell->outputCursorX = 0;
    pShell->outputCursorY = 0;
    pShell->currentLine = 0;

    for (unsigned i = firstChar; i != endChar; ++i, i %= DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS)
    {
        if (pShell->currentLine == pShell->numLines)
            return;

        DexprOS_Shell_Char character = pShell->displayCharacters[i];


        if (onScreenIndex < DEXPROS_SHELL_NUM_MAX_DISPLAY_CHARACTERS)
        {
            pShell->onScreenCharacters[onScreenIndex] = character;
            onScreenIndex += 1;
        }


        if (character == 10) // New line character
        {
            pShell->outputCursorX = 0;
            pShell->outputCursorY += lineHeight;
            pShell->currentLine += 1;
            continue;
        }

        int advanceX = DexprOS_BuiltInBitmapFontGetGlyphAdvanceX(character);

        if (pShell->outputCursorX + advanceX > (int)pShell->consoleWidth)
        {
            pShell->outputCursorX = 0;
            pShell->outputCursorY += lineHeight;
            pShell->currentLine += 1;
        }

        DexprOS_BuiltInBitmapFontDrawGlyph(pShell->pGraphicsDrv,
                                           character,
                                           pShell->outputCursorX,
                                           pShell->outputCursorY);
        pShell->outputCursorX += advanceX;
    }
}


static bool DexprOS_ShellCheckSingleCommand(const DexprOS_Shell_Char* pCommand,
                                            size_t cmdLength,
                                            size_t cmdIndex)
{
    size_t i = 0;

    for (; i < cmdLength; ++i)
    {
        char cmdCharacter = g_DexprOS_ShellBuiltInCommands[cmdIndex].commandText[i];

        if (cmdCharacter == '\0')
        {
            if (pCommand[i] == ((DexprOS_Shell_Char)' ')) // Space
            {
                return true;
            }
            return false;
        }

        if ((DexprOS_Shell_Char)cmdCharacter != pCommand[i])
            return false;
    }

    if (g_DexprOS_ShellBuiltInCommands[cmdIndex].commandText[i] != '\0')
        return false;

    return true;
}

static void DexprOS_ShellExtractArguments(const DexprOS_Shell_Char* pCommand,
                                          size_t cmdLength,
                                          const DexprOS_Shell_Char** pOutArguments,
                                          size_t* pOutArgsLength)
{
    const DexprOS_Shell_Char* pArguments = pCommand;
    size_t numCharsLeft = cmdLength;

    while (numCharsLeft > 0)
    {
        if (pArguments[0] != ((DexprOS_Shell_Char)' ')) // Space
        {
            pArguments += 1;
            numCharsLeft -= 1;
        }
        else
            break;
    }
    while (numCharsLeft > 0)
    {
        if (pArguments[0] == ((DexprOS_Shell_Char)' ')) // Space
        {
            pArguments += 1;
            numCharsLeft -= 1;
        }
        else
            break;
    }
    
    *pOutArguments = pArguments;
    *pOutArgsLength = numCharsLeft;
}

static void DexprOS_ShellInterpretCommand(DexprOS_Shell* pShell,
                                          const DexprOS_Shell_Char* pCommand,
                                          size_t cmdLength)
{
    // Skip spaces at the beginning
    while (cmdLength > 0)
    {
        if (pCommand[0] == ((DexprOS_Shell_Char)' ')) // Space
        {
            pCommand += 1;
            cmdLength -= 1;
        }
        else
            break; 
    }
    if (cmdLength == 0)
        return;
    
    // Comment message
    if (pCommand[0] == ((DexprOS_Shell_Char)'#'))
        return;


    for (size_t iCmd = 0; iCmd < DEXPROS_SHELL_NUM_BUILTIN_COMMANDS; ++iCmd)
    {
        if (DexprOS_ShellCheckSingleCommand(pCommand, cmdLength, iCmd))
        {
            const DexprOS_Shell_Char* pArguments = NULL;
            size_t argumentsLength = 0;

            DexprOS_ShellExtractArguments(pCommand, cmdLength,
                                          &pArguments, &argumentsLength);


            g_DexprOS_ShellBuiltInCommands[iCmd].commandFunc(pShell,
                                                             pArguments,
                                                             argumentsLength);
            return;
        }
    }

    DexprOS_ShellPuts(pShell, "Shell error: unknown command.\n");
}


void DexprOS_ShellFlushStdOut(DexprOS_Shell* pShell)
{
    unsigned numLinesToScroll = DexprOS_ShellGetNumOverflowLines(pShell);

    DexprOS_ShellResetText(pShell);

    DexprOS_ShellScrollDisplay(pShell, numLinesToScroll);

    DexprOS_ShellSetText(pShell,
                         pShell->firstDisplayChar,
                         pShell->displayRingBufferEnd);

    pShell->pGraphicsDrv->swapBuffers(pShell->pGraphicsDrv->pDriverData);
}


void DexprOS_ShellActivatePrompt(DexprOS_Shell* pShell)
{
    pShell->commandPromptEnabled = true;
    DexprOS_ShellPuts(pShell, pShell->promptText);
    DexprOS_ShellFlushStdOut(pShell);
}


void DexprOS_ShellKeyboardCharacterEvent(DexprOS_Shell* pShell,
                                         DexprOS_UnicodeCodepoint codepoint)
{
    if (!pShell->commandPromptEnabled)
        return;

    if (codepoint == 10) // Enter command
    {
        DexprOS_ShellPutChar(pShell, 10); // New line


        DexprOS_ShellInterpretCommand(pShell,
                                      pShell->commandCharacters,
                                      pShell->numCommandCharacters);


        for (size_t i = 0; i < DEXPROS_SHELL_NUM_MAX_COMMAND_CHARACTERS; ++i)
            pShell->commandCharacters[i] = 0;
        pShell->numCommandCharacters = 0;

        DexprOS_ShellPuts(pShell, pShell->promptText);

        DexprOS_ShellFlushStdOut(pShell);
        return;
    }


    if (pShell->numCommandCharacters == DEXPROS_SHELL_NUM_MAX_COMMAND_CHARACTERS)
        return;

    
    pShell->commandCharacters[pShell->numCommandCharacters] = codepoint;
    pShell->numCommandCharacters += 1;


    DexprOS_ShellPutChar(pShell, codepoint);
    DexprOS_ShellFlushStdOut(pShell);
}

void DexprOS_ShellKeyboardKeyEvent(DexprOS_Shell* pShell,
                                   DexprOS_KeyID keyID,
                                   DexprOS_KeyState state)
{
    if (!pShell->commandPromptEnabled)
        return;


    if (keyID == DEXPROS_KEY_BACKSPACE &&
        (state == DEXPROS_KEY_STATE_PRESSED || state == DEXPROS_KEY_STATE_REPEAT))
    {
        if (pShell->numCommandCharacters == 0)
            return;

        pShell->numCommandCharacters -= 1;
        pShell->commandCharacters[pShell->numCommandCharacters] = 0;

        DexprOS_ShellDeleteLastDisplayChar(pShell);
        DexprOS_ShellFlushStdOut(pShell);
    }
}


static void DexprOS_ShellEcho(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength)
{
    for (size_t i = 0; i < argLength; ++i)
        DexprOS_ShellPutChar(pShell, arg[i]);

    DexprOS_ShellPutChar(pShell, '\n');
}

static void DexprOS_ShellClear(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength)
{
    pShell->commandPromptEnabled = false;

    DexprOS_ShellResetText(pShell);

    pShell->displayRingBufferStart = 0;
    pShell->displayRingBufferEnd = 0;
    pShell->firstDisplayChar = 0;
    pShell->outputCursorX = 0;
    pShell->outputCursorY = 0;
    pShell->currentLine = 0;

    pShell->commandPromptEnabled = true;

    (void)arg;
    (void)argLength;
}

static void DexprOS_ShellHelp(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength)
{
    DexprOS_ShellPuts(pShell, "DexprOS version: pre-release 0.1.0\n"
                              "User help menu:\n\n"
                              "After typing a command and its space-separated arguments (if any), hit Enter to execute.\n"
                              "Lines starting with '#' are comments and are not interpreted as commands.\n"
                              "\n"
                              "List of all avaible commands:\n"
                              "help - displays this message\n"
                              "echo [MESSAGE] - prints a given message on the screen\n"
                              "clear - clears the screen\n"
                              "shutdown - powers off the computer\n"
                              "reboot - reboots the computer\n");

    (void)arg;
    (void)argLength;
}

static void DexprOS_ShellShutdown(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength)
{
    pShell->pUefiSystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, L"");

    (void)arg;
    (void)argLength;
}

static void DexprOS_ShellReboot(DexprOS_Shell* pShell, const DexprOS_Shell_Char* arg, size_t argLength)
{
    pShell->pUefiSystemTable->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, L"");

    (void)arg;
    (void)argLength;
}



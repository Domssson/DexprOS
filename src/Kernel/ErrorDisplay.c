#include "DexprOS/Kernel/ErrorDisplay.h"

#include "DexprOS/Kernel/KernelPanic.h"
#include "DexprOS/Kernel/BuiltInBitmapFont.h"

#include <stddef.h>


static DexprOS_GraphicsDriver* g_pGraphicsDrv = NULL;
static unsigned g_textConsoleWidth = 0;


static void DexprOS_KernelErrorWriteSingle(const char* text, int* pX, int* pY)
{
    unsigned lineHeight = DexprOS_BuiltInBitmapFontGetLineHeight();

    for (const char* pChar = text; *pChar != '\0'; pChar += 1)
    {
        DexprOS_UnicodeCodepoint character = (DexprOS_UnicodeCodepoint)*pChar;

        int advanceX = DexprOS_BuiltInBitmapFontGetGlyphAdvanceX(character);

        if (character == 10) // New line character
        {
            *pX = 0;
            *pY += lineHeight;
            continue;
        }
        if (*pX + advanceX > (int)g_textConsoleWidth)
        {
            *pX = 0;
            *pY += lineHeight;
        }
        DexprOS_BuiltInBitmapFontDrawGlyph(g_pGraphicsDrv,
                                           character,
                                           *pX, *pY);
        *pX += advanceX;
    }
}

static void DexprOS_KernelErrorWrite(const char* textReason)
{
    DexprOS_GrGraphicsColor color = {.r = 255, .g = 0, .b = 0};
    g_pGraphicsDrv->clearScreen(g_pGraphicsDrv->pDriverData, color);

    int x = 0;
    int y = 0;
    
    DexprOS_KernelErrorWriteSingle("KERNEL PANIC: ", &x, &y);

    DexprOS_KernelErrorWriteSingle(textReason, &x, &y);

    g_pGraphicsDrv->swapBuffers(g_pGraphicsDrv->pDriverData);
}


void DexprOS_InitKernelErrorDisplay(DexprOS_GraphicsDriver* pGraphicsDrv,
                                    unsigned textConsoleWidth)
{
    g_pGraphicsDrv = pGraphicsDrv;
    g_textConsoleWidth = textConsoleWidth;
    DexprOS_SetupKernelErrorPrintFunction(DexprOS_KernelErrorWrite);
}


#include "DexprOS/Kernel/BuiltInBitmapFont.h"

#include "DexprOS/font256.h"


void DexprOS_BuiltInBitmapFontDrawGlyph(DexprOS_GraphicsDriver* pGraphicsDrv,
                                        DexprOS_UnicodeCodepoint character,
                                        int coordX,
                                        int coordY)
{
    if (character == 0)
        return;

    // New line
    if (character == 10)
        return;
    // Skip other control characters
    if (character < 32)
        return;
    // Skip non-ASCII characters
    if (character > 127)
        return;

    const DexprOS_UnicodeCodepoint printableASCIIIndex = character - 32;

    const DexprOS_FontCharacterData charData = g_DexprOS_FontMetrics.characters[printableASCIIIndex];

    int startX = coordX + charData.bitmapLeft;
    int startY = coordY - charData.bitmapTop +
                 g_DexprOS_FontMetrics.lineHeight;
    

    pGraphicsDrv->blitImageToFramebuffer(pGraphicsDrv->pDriverData,
                                         DexprOS_FontImageData,
                                         DexprOS_FontImageFormat,
                                         charData.imageOffsetX,
                                         charData.imageOffsetY,
                                         DexprOS_FontImageRowStride,
                                         startX,
                                         startY,
                                         charData.width,
                                         charData.height);
}

int DexprOS_BuiltInBitmapFontGetGlyphAdvanceX(DexprOS_UnicodeCodepoint character)
{
    if (character == 0 || character == 10) // Null or new line
        return 0;

    // Skip other control characters
    if (character < 32)
        return 0;
    // Skip non-ASCII characters
    if (character > 127)
        return 0;

    const DexprOS_UnicodeCodepoint printableASCIIIndex = character - 32;

    return g_DexprOS_FontMetrics.characters[printableASCIIIndex].advanceX;
}

DexprOS_BuiltInBitmapFontMetrics
DexprOS_BuiltInBitmapFontGetGlyphMetrics(DexprOS_UnicodeCodepoint character)
{
    DexprOS_BuiltInBitmapFontMetrics dummyMetrics = {
        .offsetX = 0,
        .offsetY = 0,
        .width = 0,
        .height = 0,
        .advanceX = 0
    };

    if (character == 0 || character == 10) // Null or new line
        return dummyMetrics;

    // Skip other control characters
    if (character < 32)
        return dummyMetrics;
    // Skip non-ASCII characters
    if (character > 127)
        return dummyMetrics;

    const DexprOS_UnicodeCodepoint printableASCIIIndex = character - 32;

    const DexprOS_FontCharacterData charData = g_DexprOS_FontMetrics.characters[printableASCIIIndex];

    DexprOS_BuiltInBitmapFontMetrics metrics = {
        .offsetX = charData.bitmapLeft,
        .offsetY = g_DexprOS_FontMetrics.lineHeight - charData.bitmapTop,
        .width = charData.width,
        .height = charData.height,
        .advanceX = charData.advanceX
    };
    return metrics;
}

unsigned DexprOS_BuiltInBitmapFontGetLineHeight(void)
{
    return g_DexprOS_FontMetrics.lineHeight;
}


#ifndef DEXPROS_KERNEL_BUILTINBITMAPFONT_H_INCLUDED
#define DEXPROS_KERNEL_BUILTINBITMAPFONT_H_INCLUDED

#include "DexprOS/Drivers/Graphics/GraphicsDriver.h"
#include "DexprOS/DexprOSCommon.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_BuiltInBitmapFontMetrics
{
    int offsetX;
    int offsetY;
    unsigned width;
    unsigned height;

    int advanceX;
} DexprOS_BuiltInBitmapFontMetrics;


void DexprOS_BuiltInBitmapFontDrawGlyph(DexprOS_GraphicsDriver* pGraphicsDrv,
                                        DexprOS_UnicodeCodepoint character,
                                        int coordX,
                                        int coordY);

int DexprOS_BuiltInBitmapFontGetGlyphAdvanceX(DexprOS_UnicodeCodepoint character);

DexprOS_BuiltInBitmapFontMetrics
DexprOS_BuiltInBitmapFontGetGlyphMetrics(DexprOS_UnicodeCodepoint character);

unsigned DexprOS_BuiltInBitmapFontGetLineHeight(void);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_BUILTINBITMAPFONT_H_INCLUDED

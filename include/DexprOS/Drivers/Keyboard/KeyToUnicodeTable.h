#ifndef DEXPROS_DRIVERS_KEYBOARD_KEYTOUNICODETABLE_H_INCLUDED
#define DEXPROS_DRIVERS_KEYBOARD_KEYTOUNICODETABLE_H_INCLUDED

#include "DexprOS/Kernel/KeyboardInput.h"
#include "DexprOS/DexprOSCommon.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum DexprOS_KeyToUnicodeConversionType
{
    DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID = 0,
    DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED,
    DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED,
    DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD,
    DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_ENTER
} DexprOS_KeyToUnicodeConversionType;


typedef struct DexprOS_KeyToUnicodeEntry
{
    DexprOS_KeyToUnicodeConversionType conversionType;

    DexprOS_UnicodeCodepoint standardCode;
    DexprOS_UnicodeCodepoint upperCaseCode;
    DexprOS_UnicodeCodepoint altGrCode;
    DexprOS_UnicodeCodepoint altGrUpperCaseCode;
} DexprOS_KeyToUnicodeEntry;


typedef struct DexprOS_KeyToUnicodeTable
{
    DexprOS_KeyToUnicodeEntry table[DEXPROS_KEY_Max];
} DexprOS_KeyToUnicodeTable;


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_DRIVERS_KEYBOARD_KEYTOUNICODETABLE_H_INCLUDED

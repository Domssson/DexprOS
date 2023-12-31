#include "DexprOS/Drivers/Keyboard/USKeyboardLayout.h"

#include "DexprOS/Drivers/Keyboard/KeyToUnicodeTable.h"
#include "DexprOS/Kernel/KeyboardInput.h"


static DexprOS_KeyToUnicodeTable g_conversionTableUS = {
    .table = {
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 32, 32, 32, 32},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 39, 34, 39, 34},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 44, 60, 44, 60},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 45, 95, 45, 95},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 46, 62, 46, 62},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 47, 63, 47, 63},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 48, 41, 48, 41},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 49, 33, 49, 33},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 50, 64, 50, 64},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 51, 35, 51, 35},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 52, 36, 52, 36},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 53, 37, 53, 37},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 54, 94, 54, 94},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 55, 38, 55, 38},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 56, 42, 56, 42},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 57, 40, 57, 40},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 59, 58, 59, 58},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 61, 43, 61, 43},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 97, 65, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 98, 66, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 99, 67, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 100, 68, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 101, 69, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 102, 70, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 103, 71, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 104, 72, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 105, 73, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 106, 74, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 107, 75, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 108, 76, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 109, 77, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 110, 78, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 111, 79, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 112, 80, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 113, 81, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 114, 82, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 115, 83, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 116, 84, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 117, 85, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 118, 86, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 119, 87, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 120, 88, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 121, 89, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED, 122, 90, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 91, 123, 91, 123},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 92, 124, 92, 124},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 93, 125, 93, 125},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 96, 126, 96, 126},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_ENTER, 10, 10, 10, 10},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 48, 48, 48, 48},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 49, 49, 49, 49},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 50, 50, 50, 50},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 51, 51, 51, 51},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 52, 52, 52, 52},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 53, 53, 53, 53},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 54, 54, 54, 54},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 55, 55, 55, 55},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 56, 56, 56, 56},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 57, 57, 57, 57},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD, 46, 46, 46, 46},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 47, 47, 47, 47},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 42, 42, 42, 42},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 45, 45, 45, 45},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 43, 43, 43, 43},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_ENTER, 10, 10, 10, 10},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED, 61, 61, 61, 61},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0},
        {DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID, 0, 0, 0, 0}
    }
};


const DexprOS_KeyToUnicodeTable* DexprOS_GetKeyboardLayout_US(void)
{
    return &g_conversionTableUS;
}


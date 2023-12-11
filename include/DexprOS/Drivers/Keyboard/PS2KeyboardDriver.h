#ifndef DEXPROS_DRIVERS_KEYBOARD_PS2KEYBOARDDRIVER_H_INCLUDED
#define DEXPROS_DRIVERS_KEYBOARD_PS2KEYBOARDDRIVER_H_INCLUDED

#include "KeyToUnicodeTable.h"

#ifdef __cplusplus
extern "C"
{
#endif


// Interrupts must be disabled when calling this function!
void DexprOS_InitialisePS2KeyboardDriver(const DexprOS_KeyToUnicodeTable* pLayoutTable);

void DexprOS_ProcessPS2KeyboardInterrupt(void);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_DRIVERS_KEYBOARD_PS2KEYBOARDDRIVER_H_INCLUDED

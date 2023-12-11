#ifndef DEXPROS_DRIVERS_PICDRIVER_H_INCLUDED
#define DEXPROS_DRIVERS_PICDRIVER_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


// Initialises the two Programmable Interrupt Controllers and
// changes their IDT entry offsets
void DexprOS_InitialisePIC(uint8_t int0Offset, uint8_t int1Offset);

// Sends the End of Interrupt command to the PICs in order to
// make further PIC interrupt retrieval possible.
void DexprOS_PIC_SendEOI(uint8_t irqNumber);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_DRIVERS_PICDRIVER_H_INCLUDED

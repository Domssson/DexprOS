#ifndef DEXPROS_KERNEL_ERRORDISPLAY_H_INCLUDED
#define DEXPROS_KERNEL_ERRORDISPLAY_H_INCLUDED

#include "DexprOS/Drivers/Graphics/GraphicsDriver.h"

#ifdef __cplusplus
extern "C"
{
#endif


void DexprOS_InitKernelErrorDisplay(DexprOS_GraphicsDriver* pGraphicsDrv,
                                    unsigned textConsoleWidth);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_ERRORDISPLAY_H_INCLUDED

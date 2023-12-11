#include "DexprOS/Kernel/KernelPanic.h"

#include "DexprOS/Kernel/x86_64/Interrupts.h"
#include "DexprOS/Kernel/x86_64/KernelPanicHalt.h"

#include <stddef.h>


static DexprOS_KernelErrorPrintFunc g_kernelErrorPrintFunc = NULL;

void DexprOS_SetupKernelErrorPrintFunction(DexprOS_KernelErrorPrintFunc func)
{
    g_kernelErrorPrintFunc = func;
}


void DexprOS_KernelPanic(const char* textReason)
{
    DexprOS_DisableInterrupts();

    if (g_kernelErrorPrintFunc != NULL)
    {
        g_kernelErrorPrintFunc(textReason);
    }

    DexprOS_KernelPanicHaltMachine();
}

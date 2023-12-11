#ifndef DEXPROS_KERNEL_KERNELPANIC_H_INCLUDED
#define DEXPROS_KERNEL_KERNELPANIC_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


typedef void (*DexprOS_KernelErrorPrintFunc)(const char* textReason);

void DexprOS_SetupKernelErrorPrintFunction(DexprOS_KernelErrorPrintFunc func);


void DexprOS_KernelPanic(const char* textReason);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_KERNELPANIC_H_INCLUDED

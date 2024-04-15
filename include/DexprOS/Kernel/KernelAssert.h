#ifndef DEXPROS_KERNEL_KERNELASSERT_H_INCLUDED
#define DEXPROS_KERNEL_KERNELASSERT_H_INCLUDED

#include "DexprOS/Kernel/KernelPanic.h"


#define DEXPROS_KERNEL_ASSERT_DETAIL_LINE_TO_STRING2(x) #x
#define DEXPROS_KERNEL_ASSERT_DETAIL_LINE_TO_STRING(x) DEXPROS_KERNEL_ASSERT_DETAIL_LINE_TO_STRING2(x)


#define DEXPROS_KERNEL_ASSERT(x)                                                                    \
do                                                                                                  \
{                                                                                                   \
    if (!(x))                                                                                       \
        DexprOS_KernelPanic("DexprOS Kernel assertion failed: " #x                                  \
                            ", file: " __FILE__                                                     \
                            ", line: " DEXPROS_KERNEL_ASSERT_DETAIL_LINE_TO_STRING(__LINE__) "\n"); \
}                                                                                                   \
while(0)


#endif // DEXPROS_KERNEL_KERNELASSERT_H_INCLUDED

#ifndef DEXPROS_DEXPROS_COMMON_H_INCLUDED
#define DEXPROS_DEXPROS_COMMON_H_INCLUDED

#include <stdint.h>


#ifndef DEXPROS_ALIGNAS
#   if defined(__cplusplus)
#       if __cplusplus >= 201103L
#           define DEXPROS_ALIGNAS(x) alignas(x)
#       endif
#   elif defined(__STDC_VERSION__)
#       if __STDC_VERSION__ >= 201112L
#           define DEXPROS_ALIGNAS(x) _Alignas(x)
#       endif
#   endif
#endif

#ifndef DEXPROS_ALIGNAS
#   if defined(__GNUC__) || defined(__clang__)
#       define DEXPROS_ALIGNAS(x) __attribute__((aligned(x)))
#   else
#       error Compiling under unsupported compiler
#   endif
#endif


typedef uint32_t DexprOS_UnicodeCodepoint;


#endif // DEXPROS_DEXPROS_COMMON_H_INCLUDED

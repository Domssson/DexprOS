#include "DexprOS/Kernel/x86_64/FloatingPointInit.h"


// Declare floating point used symbol for clang.
//
// In user-land programs, _fltused is a symbol injected by clang when a program
// uses floating point operations, so that the linker knows that it must link
// floating point init code. This isn't the case for freestanding code.
//
// Instead, floating point operations are initialised in FloatingPointInit.s.
//
// So declare the variable so clang doesn't complain about an undefined symbol.
#ifdef __clang__
int _fltused = 0;
#endif


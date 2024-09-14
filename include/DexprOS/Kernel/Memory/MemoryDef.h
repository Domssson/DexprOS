#ifndef DEXPROS_KERNEL_MEMORY_MEMORYDEF_H_INCLUDED
#define DEXPROS_KERNEL_MEMORY_MEMORYDEF_H_INCLUDED

#include "Paging.h"


// Buffers allocated with this alignment can be used to store any object of
// standard alignment requirements (fundamental types, structs, unions, enums).
#define DEXPROS_FUNDAMENTAL_ALIGNMENT 16


// a must be power of 2
#define DEXPROS_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))


#define DEXPROS_ALIGN_FUNDAMENTAL(x) DEXPROS_ALIGN((x), DEXPROS_FUNDAMENTAL_ALIGNMENT)


#endif // DEXPROS_KERNEL_MEMORY_MEMORYDEF_H_INCLUDED

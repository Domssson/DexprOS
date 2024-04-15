#ifndef DEXPROS_KERNEL_KSTDLIB_STRING_H_INCLUDED
#define DEXPROS_KERNEL_KSTDLIB_STRING_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


void* memset(void* dest, int ch, size_t count);


#if __STDC_VERSION__ >= 199901L // C99

void* memcpy(void* restrict dest, const void* restrict src, size_t count);

#else

void* memcpy(void* dest, const void* src, size_t count);

#endif


void* memmove(void* dest, const void* src, size_t count);


int memcmp(const void* lhs, const void* rhs, size_t count);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEXPROS_KERNEL_KSTDLIB_STRING_H_INCLUDED

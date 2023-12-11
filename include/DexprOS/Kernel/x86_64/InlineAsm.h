#ifndef DEXPROS_KERNEL_X86_64_INLINEASM_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_INLINEASM_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Inline assembly functions */


static inline void outb(uint8_t value, uint16_t port)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_INLINEASM_H_INCLUDED

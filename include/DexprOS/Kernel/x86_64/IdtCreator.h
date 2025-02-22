#ifndef DEXPROS_KERNEL_X86_64_IDTCREATOR_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_IDTCREATOR_H_INCLUDED

#include "DexprOS/Kernel/Memory/MemoryDef.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum DexprOS_InterruptFlags
{
    // If set on an interrupt vector, it serves as an interrupt gate
    // used for hardware and software interrupts.
    DEXPROS_INTERRUPT_GATE_TYPE_INTERRUPT_GATE_BITS = 0xE,
    // If set on an interrupt vector, it serves as a trap gate
    // used for exception handling.
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS = 0xF,

    // Determines if a user space app can fire this interrupt. Ignored
    // by hardware interrupts.
    DEXPROS_INTERRUPT_USERSPACE_ACCESS_BIT = 0x60,

    // Should always be set on a valid interrupt vector.
    DEXPROS_INTERRUPT_PRESENT_BIT = 0x80
} DexprOS_InterruptGateType;


typedef struct DexprOS_InterruptDescriptor
{
    // Offset to the Interrupt Service Routine entry point
    uint64_t offset;
    // The segment selector to load from GDT
    uint16_t segmentSelector;
    // The stack pointer to load from task state segment or 0
    uint8_t ist;
    
    uint8_t flags;
} DexprOS_InterruptDescriptor;


void DexprOS_EncodeIDTEntry(uint8_t entryIndex,
                            const DexprOS_InterruptDescriptor* pDesc);

void DexprOS_SetupIDT(DexprOS_VirtualMemoryAddress rellocOffset,
                      uint16_t kernelCodeSegment);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_IDTCREATOR_H_INCLUDED

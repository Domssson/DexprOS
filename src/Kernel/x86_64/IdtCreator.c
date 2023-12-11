#include "DexprOS/Kernel/x86_64/IdtCreator.h"

#include "DexprOS/Kernel/x86_64/Interrupts.h"

#include "DexprOS/DexprOSCommon.h"


#define DEXPROS_X86_64_NUM_IDT_ENTRIES 256


DEXPROS_ALIGNAS(16)
static uint64_t g_DexprOS_InterruptDescriptorTable[DEXPROS_X86_64_NUM_IDT_ENTRIES * 2];


void DexprOS_EncodeIDTEntry(uint8_t entryIndex,
                            const DexprOS_InterruptDescriptor* pDesc)
{
    const uint16_t i = ((uint16_t)entryIndex) * 2;

    g_DexprOS_InterruptDescriptorTable[i] = 0;
    g_DexprOS_InterruptDescriptorTable[i + 1] = 0;

    const uint64_t offset = pDesc->offset;
    const uint64_t segment = pDesc->segmentSelector;
    const uint64_t ist = pDesc->ist;
    const uint64_t flags = pDesc->flags;

    g_DexprOS_InterruptDescriptorTable[i] |= (offset & 0xFFFF);
    g_DexprOS_InterruptDescriptorTable[i] |= (segment << 16);
    g_DexprOS_InterruptDescriptorTable[i] |= ((ist & 0x7) << 32);
    g_DexprOS_InterruptDescriptorTable[i] |= (flags << 40);
    g_DexprOS_InterruptDescriptorTable[i] |= (((offset >> 16) & 0xFFFF) << 48);
    g_DexprOS_InterruptDescriptorTable[i + 1] |= (offset >> 32);
}


static const uint64_t g_DexprOS_ExceptionISRAdresses[32] = {
    (uint64_t)DexprOS_ISR_DivisionError,
    (uint64_t)DexprOS_ISR_Debug,
    (uint64_t)DexprOS_ISR_NMI,
    (uint64_t)DexprOS_ISR_Breakpoint,
    (uint64_t)DexprOS_ISR_Overflow,
    (uint64_t)DexprOS_ISR_BoundRangeExceeded,
    (uint64_t)DexprOS_ISR_InvalidOpcode,
    (uint64_t)DexprOS_ISR_DeviceNotAvaible,
    (uint64_t)DexprOS_ISR_DoubleFault,
    0,
    (uint64_t)DexprOS_ISR_InvalidTSS,
    (uint64_t)DexprOS_ISR_SegmentNotPresent,
    (uint64_t)DexprOS_ISR_StackSegmentFault,
    (uint64_t)DexprOS_ISR_GeneralProtectionFault,
    (uint64_t)DexprOS_ISR_PageFault,
    0,
    (uint64_t)DexprOS_ISR_x87FloatingPointException,
    (uint64_t)DexprOS_ISR_AlignmentCheck,
    (uint64_t)DexprOS_ISR_MachineCheck,
    (uint64_t)DexprOS_ISR_SIMDException,
    (uint64_t)DexprOS_ISR_VirtualizationException,
    (uint64_t)DexprOS_ISR_ControlProtectionException,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint64_t)DexprOS_ISR_HypervisorInjectionException,
    (uint64_t)DexprOS_ISR_VMMCommunicationException,
    (uint64_t)DexprOS_ISR_SecurityException,
    0
};

static const uint8_t g_DexprOS_IDTExceptionEntryFlags[32] = {
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_INTERRUPT_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    0,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    0,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    0,
    0,
    0,
    0,
    0,
    0,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    DEXPROS_INTERRUPT_GATE_TYPE_TRAP_GATE_BITS | DEXPROS_INTERRUPT_PRESENT_BIT,
    0
};

static const uint64_t g_DexprOS_ISA_ISR_Addresses[16] = {
    (uint64_t)DexprOS_ISR_ISA_PIT,
    (uint64_t)DexprOS_ISR_ISA_Keyboard,
    (uint64_t)DexprOS_ISR_ISA_Stub,
    (uint64_t)DexprOS_ISR_ISA_COM2,
    (uint64_t)DexprOS_ISR_ISA_COM1,
    (uint64_t)DexprOS_ISR_ISA_LPT2,
    (uint64_t)DexprOS_ISR_ISA_FloppyDisk,
    (uint64_t)DexprOS_ISR_ISA_LPT1_Spurious,
    (uint64_t)DexprOS_ISR_ISA_CMOSClock,
    (uint64_t)DexprOS_ISR_ISA_Peripherals0,
    (uint64_t)DexprOS_ISR_ISA_Peripherals1,
    (uint64_t)DexprOS_ISR_ISA_Peripherals2,
    (uint64_t)DexprOS_ISR_ISA_PS2Mouse,
    (uint64_t)DexprOS_ISR_ISA_FPU,
    (uint64_t)DexprOS_ISR_ISA_PrimaryATAHardDisk,
    (uint64_t)DexprOS_ISR_ISA_SecondaryATAHardDisk
};


void DexprOS_SetupIDT(uint16_t kernelCodeSegment)
{
    // Encode exception ISR entries
    for (unsigned i = 0; i < 32; ++i)
    {
        DexprOS_InterruptDescriptor desc = {0};
        desc.offset = g_DexprOS_ExceptionISRAdresses[i];
        desc.flags = g_DexprOS_IDTExceptionEntryFlags[i];
        desc.ist = 0;

        if ((desc.flags & DEXPROS_INTERRUPT_PRESENT_BIT) == DEXPROS_INTERRUPT_PRESENT_BIT)
            desc.segmentSelector = kernelCodeSegment;

        DexprOS_EncodeIDTEntry(i, &desc);
    }
    // Encode ISA ISR entries
    for (unsigned i = 0; i < 16; ++i)
    {
        DexprOS_InterruptDescriptor desc = {0};
        desc.offset = g_DexprOS_ISA_ISR_Addresses[i];
        desc.segmentSelector = kernelCodeSegment;
        desc.ist = 0;
        desc.flags = DEXPROS_INTERRUPT_GATE_TYPE_INTERRUPT_GATE_BITS |
                     DEXPROS_INTERRUPT_PRESENT_BIT;
        
        DexprOS_EncodeIDTEntry(32 + i, &desc);
    }

    // Encode unsused interrupts 48-255
    for (unsigned i = 48; i < 256; ++i)
    {
        DexprOS_InterruptDescriptor desc = {
            .offset = 0,
            .segmentSelector = 0,
            .ist = 0,
            .flags = 0
        };
        DexprOS_EncodeIDTEntry(i, &desc);
    }


    DexprOS_LoadIDT(sizeof(g_DexprOS_InterruptDescriptorTable) - 1,
                    (uint64_t)&g_DexprOS_InterruptDescriptorTable[0]);
}


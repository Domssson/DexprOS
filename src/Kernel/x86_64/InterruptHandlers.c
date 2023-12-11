#include "DexprOS/Kernel/x86_64/InterruptHandlers.h"

#include "DexprOS/Kernel/KernelPanic.h"

#include "DexprOS/Drivers/PICDriver.h"
#include "DexprOS/Drivers/Keyboard/PS2KeyboardDriver.h"


void DexprOS_HandleInterrupt_DivisionError(void)
{
    DexprOS_KernelPanic("division by zero error");
}

void DexprOS_HandleInterrupt_Debug(void)
{
    DexprOS_KernelPanic("received a debug interrupt");
}

void DexprOS_HandleInterrupt_NMI(void)
{
    DexprOS_KernelPanic("received a non-maskable interrupt, which isn't supported yet");
}

void DexprOS_HandleInterrupt_Breakpoint(void)
{
    DexprOS_KernelPanic("received a breakpoint interrupt");
}

void DexprOS_HandleInterrupt_Overflow(void)
{
    DexprOS_KernelPanic("overflow exception");
}

void DexprOS_HandleInterrupt_BoundRangeExceeded(void)
{
    DexprOS_KernelPanic("bound range exceeded exception");
}

void DexprOS_HandleInterrupt_InvalidOpcode(void)
{
    DexprOS_KernelPanic("invalid program OpCode");
}

void DexprOS_HandleInterrupt_DeviceNotAvaible(void)
{
    DexprOS_KernelPanic("received a Device Not Avaible exception");
}

void DexprOS_HandleInterrupt_DoubleFault(void)
{
    DexprOS_KernelPanic("double fault: an unhandled exception occured");
}

void DexprOS_HandleInterrupt_InvalidTSS(uint64_t errorCode)
{
    (void)errorCode;
    DexprOS_KernelPanic("attempt to load an invalid Task State Segment");
}

void DexprOS_HandleInterrupt_SegmentNotPresent(uint64_t errorCode)
{
    (void)errorCode;
    DexprOS_KernelPanic("attempt to use a General Descriptor Table segment which isn't present");
}

void DexprOS_HandleInterrupt_StackSegmentFault(uint64_t errorCode)
{
    (void)errorCode;
    DexprOS_KernelPanic("stack segment fault: unsupported yet");
}

void DexprOS_HandleInterrupt_GeneralProtectionFault(uint64_t errorCode)
{
    (void)errorCode;
    DexprOS_KernelPanic("general protection fault: unsupported yet");
}

void DexprOS_HandleInterrupt_PageFault(uint64_t errorCode)
{
    (void)errorCode;
    DexprOS_KernelPanic("page fault: unsupported yet");
}

void DexprOS_HandleInterrupt_x87FloatingPointException(void)
{
    DexprOS_KernelPanic("x86 floating point exception: unsupported yet");
}

void DexprOS_HandleInterrupt_AlignmentCheck(uint64_t errorCode)
{
    (void)errorCode;
    DexprOS_KernelPanic("alignment check exception: unsupported yet");
}

void DexprOS_HandleInterrupt_MachineCheck(void)
{
    DexprOS_KernelPanic("Machine Check: internal errors in the CPU");
}



void DexprOS_HandleInterrupt_ISA_PIT(void)
{
    DexprOS_PIC_SendEOI(0);
}

void DexprOS_HandleInterrupt_ISA_Keyboard(void)
{
    DexprOS_ProcessPS2KeyboardInterrupt();

    DexprOS_PIC_SendEOI(1);
}

void DexprOS_HandleInterrupt_ISA_COM2(void)
{
    DexprOS_PIC_SendEOI(3);
}

void DexprOS_HandleInterrupt_ISA_COM1(void)
{
    DexprOS_PIC_SendEOI(4);
}

void DexprOS_HandleInterrupt_ISA_LPT2(void)
{
    DexprOS_PIC_SendEOI(5);
}

void DexprOS_HandleInterrupt_ISA_FloppyDisk(void)
{
    DexprOS_PIC_SendEOI(6);
}

void DexprOS_HandleInterrupt_ISA_LPT1_Spurious(void)
{
    // Do not send the EOI command after spurious interrupts
}

void DexprOS_HandleInterrupt_ISA_CMOSClock(void)
{
    DexprOS_PIC_SendEOI(8);
}

void DexprOS_HandleInterrupt_ISA_Peripherals0(void)
{
    DexprOS_PIC_SendEOI(9);
}

void DexprOS_HandleInterrupt_ISA_Peripherals1(void)
{
    DexprOS_PIC_SendEOI(10);
}

void DexprOS_HandleInterrupt_ISA_Peripherals2(void)
{
    DexprOS_PIC_SendEOI(11);
}

void DexprOS_HandleInterrupt_ISA_PS2Mouse(void)
{
    DexprOS_PIC_SendEOI(12);
}

void DexprOS_HandleInterrupt_ISA_FPU(void)
{
    DexprOS_PIC_SendEOI(13);
}

void DexprOS_HandleInterrupt_ISA_PrimaryATAHardDisk(void)
{
    DexprOS_PIC_SendEOI(14);
}

void DexprOS_HandleInterrupt_ISA_SecondaryATAHardDisk(void)
{
    DexprOS_PIC_SendEOI(15);
}


void DexprOS_HandleInterrupt_Stub(void)
{

}


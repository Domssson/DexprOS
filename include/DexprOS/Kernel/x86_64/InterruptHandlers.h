#ifndef DEXPROS_KERNEL_X86_64_INTERRUPTHANDLERS_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_INTERRUPTHANDLERS_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


void DexprOS_HandleInterrupt_DivisionError(void);
void DexprOS_HandleInterrupt_Debug(void);
void DexprOS_HandleInterrupt_NMI(void);
void DexprOS_HandleInterrupt_Breakpoint(void);
void DexprOS_HandleInterrupt_Overflow(void);
void DexprOS_HandleInterrupt_BoundRangeExceeded(void);
void DexprOS_HandleInterrupt_InvalidOpcode(void);
void DexprOS_HandleInterrupt_DeviceNotAvaible(void);
void DexprOS_HandleInterrupt_DoubleFault(void);
void DexprOS_HandleInterrupt_InvalidTSS(uint64_t errorCode);
void DexprOS_HandleInterrupt_SegmentNotPresent(uint64_t errorCode);
void DexprOS_HandleInterrupt_StackSegmentFault(uint64_t errorCode);
void DexprOS_HandleInterrupt_GeneralProtectionFault(uint64_t errorCode);
void DexprOS_HandleInterrupt_PageFault(uint64_t errorCode);
void DexprOS_HandleInterrupt_x87FloatingPointException(void);
void DexprOS_HandleInterrupt_AlignmentCheck(uint64_t errorCode);
void DexprOS_HandleInterrupt_MachineCheck(void);

void DexprOS_HandleInterrupt_ISA_PIT(void);
void DexprOS_HandleInterrupt_ISA_Keyboard(void);
void DexprOS_HandleInterrupt_ISA_COM2(void);
void DexprOS_HandleInterrupt_ISA_COM1(void);
void DexprOS_HandleInterrupt_ISA_LPT2(void);
void DexprOS_HandleInterrupt_ISA_FloppyDisk(void);
void DexprOS_HandleInterrupt_ISA_LPT1_Spurious(void);
void DexprOS_HandleInterrupt_ISA_CMOSClock(void);
void DexprOS_HandleInterrupt_ISA_Peripherals0(void);
void DexprOS_HandleInterrupt_ISA_Peripherals1(void);
void DexprOS_HandleInterrupt_ISA_Peripherals2(void);
void DexprOS_HandleInterrupt_ISA_PS2Mouse(void);
void DexprOS_HandleInterrupt_ISA_FPU(void);
void DexprOS_HandleInterrupt_ISA_PrimaryATAHardDisk(void);
void DexprOS_HandleInterrupt_ISA_SecondaryATAHardDisk(void);

void DexprOS_HandleInterrupt_Stub(void);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_INTERRUPTHANDLERS_H_INCLUDED

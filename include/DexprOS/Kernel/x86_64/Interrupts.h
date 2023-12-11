#ifndef DEXPROS_KERNEL_X86_64_INTERRUPTS_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_INTERRUPTS_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


extern void DexprOS_DisableInterrupts(void);

extern void DexprOS_EnableInterrupts(void);


extern void DexprOS_LoadIDT(uint16_t limit,
                            uint64_t offset);


extern void DexprOS_ISR_DivisionError(void);
extern void DexprOS_ISR_Debug(void);
extern void DexprOS_ISR_NMI(void);
extern void DexprOS_ISR_Breakpoint(void);
extern void DexprOS_ISR_Overflow(void);
extern void DexprOS_ISR_BoundRangeExceeded(void);
extern void DexprOS_ISR_InvalidOpcode(void);
extern void DexprOS_ISR_DeviceNotAvaible(void);
extern void DexprOS_ISR_DoubleFault(void);
extern void DexprOS_ISR_InvalidTSS(void);
extern void DexprOS_ISR_SegmentNotPresent(void);
extern void DexprOS_ISR_StackSegmentFault(void);
extern void DexprOS_ISR_GeneralProtectionFault(void);
extern void DexprOS_ISR_PageFault(void);
extern void DexprOS_ISR_x87FloatingPointException(void);
extern void DexprOS_ISR_AlignmentCheck(void);
extern void DexprOS_ISR_MachineCheck(void);
extern void DexprOS_ISR_SIMDException(void);
extern void DexprOS_ISR_VirtualizationException(void);
extern void DexprOS_ISR_ControlProtectionException(void);
extern void DexprOS_ISR_HypervisorInjectionException(void);
extern void DexprOS_ISR_VMMCommunicationException(void);
extern void DexprOS_ISR_SecurityException(void);

extern void DexprOS_ISR_ISA_PIT(void);
extern void DexprOS_ISR_ISA_Keyboard(void);
extern void DexprOS_ISR_ISA_Stub(void);
extern void DexprOS_ISR_ISA_COM2(void);
extern void DexprOS_ISR_ISA_COM1(void);
extern void DexprOS_ISR_ISA_LPT2(void);
extern void DexprOS_ISR_ISA_FloppyDisk(void);
extern void DexprOS_ISR_ISA_LPT1_Spurious(void);
extern void DexprOS_ISR_ISA_CMOSClock(void);
extern void DexprOS_ISR_ISA_Peripherals0(void);
extern void DexprOS_ISR_ISA_Peripherals1(void);
extern void DexprOS_ISR_ISA_Peripherals2(void);
extern void DexprOS_ISR_ISA_PS2Mouse(void);
extern void DexprOS_ISR_ISA_FPU(void);
extern void DexprOS_ISR_ISA_PrimaryATAHardDisk(void);
extern void DexprOS_ISR_ISA_SecondaryATAHardDisk(void);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_KERNEL_X86_64_INTERRUPTS_H_INCLUDED

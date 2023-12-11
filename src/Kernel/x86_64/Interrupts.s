.text

    .global DexprOS_DisableInterrupts
    .global DexprOS_EnableInterrupts
    .global DexprOS_LoadIDT

    .global DexprOS_ISR_DivisionError
    .global DexprOS_ISR_Debug
    .global DexprOS_ISR_NMI
    .global DexprOS_ISR_Breakpoint
    .global DexprOS_ISR_Overflow
    .global DexprOS_ISR_BoundRangeExceeded
    .global DexprOS_ISR_InvalidOpcode
    .global DexprOS_ISR_DeviceNotAvaible
    .global DexprOS_ISR_DoubleFault
    .global DexprOS_ISR_InvalidTSS
    .global DexprOS_ISR_SegmentNotPresent
    .global DexprOS_ISR_StackSegmentFault
    .global DexprOS_ISR_GeneralProtectionFault
    .global DexprOS_ISR_PageFault
    .global DexprOS_ISR_x87FloatingPointException
    .global DexprOS_ISR_AlignmentCheck
    .global DexprOS_ISR_MachineCheck
    .global DexprOS_ISR_SIMDException
    .global DexprOS_ISR_VirtualizationException
    .global DexprOS_ISR_ControlProtectionException
    .global DexprOS_ISR_HypervisorInjectionException
    .global DexprOS_ISR_VMMCommunicationException
    .global DexprOS_ISR_SecurityException

    .global DexprOS_ISR_ISA_PIT
    .global DexprOS_ISR_ISA_Keyboard
    .global DexprOS_ISR_ISA_Stub
    .global DexprOS_ISR_ISA_COM2
    .global DexprOS_ISR_ISA_COM1
    .global DexprOS_ISR_ISA_LPT2
    .global DexprOS_ISR_ISA_FloppyDisk
    .global DexprOS_ISR_ISA_LPT1_Spurious
    .global DexprOS_ISR_ISA_CMOSClock
    .global DexprOS_ISR_ISA_Peripherals0
    .global DexprOS_ISR_ISA_Peripherals1
    .global DexprOS_ISR_ISA_Peripherals2
    .global DexprOS_ISR_ISA_PS2Mouse
    .global DexprOS_ISR_ISA_FPU
    .global DexprOS_ISR_ISA_PrimaryATAHardDisk
    .global DexprOS_ISR_ISA_SecondaryATAHardDisk

    .global DexprOS_ISR_SystemCall


DexprOS_DisableInterrupts:
    cli
    ret


DexprOS_EnableInterrupts:
    sti
    ret


DexprOS_LoadIDT:
    /* Push the two first arguments on to the stack to form an 10-byte IDTR value */
    pushq %rdx
    pushw %cx

    /* Load the value to Interrupt Descriptor Table Register */
    lidt (%rsp)
    
    /* Pop unnecessary variables from the stack */
    popw %cx
    popq %rdx

    ret


/* Beginning of the exception ISRs */


DexprOS_ISR_DivisionError:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_DivisionError

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_Debug:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_Debug

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_NMI:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_NMI

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_Breakpoint:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_Breakpoint

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_Overflow:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_Overflow

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_BoundRangeExceeded:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_BoundRangeExceeded

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_InvalidOpcode:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_InvalidOpcode

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_DeviceNotAvaible:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_DeviceNotAvaible

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_DoubleFault:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_DoubleFault

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop the (always 0) error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_InvalidTSS:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_InvalidTSS

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_SegmentNotPresent:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_SegmentNotPresent

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_StackSegmentFault:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_StackSegmentFault

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_GeneralProtectionFault:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_GeneralProtectionFault

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_PageFault:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_PageFault

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_x87FloatingPointException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_x87FloatingPointException

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_AlignmentCheck:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_AlignmentCheck

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_MachineCheck:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_MachineCheck

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_SIMDException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_Stub

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_VirtualizationException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_Stub

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_ControlProtectionException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_Stub

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_HypervisorInjectionException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_Stub

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


DexprOS_ISR_VMMCommunicationException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_Stub

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


DexprOS_ISR_SecurityException:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    /* Pass the error code from the stack to the function */
    mov 56(%rsp), %rcx
    call DexprOS_HandleInterrupt_Stub

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax

    /* Pop error code from the stack */
    add $8, %rsp

    iretq


/* ISA (Industry Standard Architecture) ISRs*/

DexprOS_ISR_ISA_PIT:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_PIT

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_Keyboard:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_Keyboard

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_Stub:
    iretq

DexprOS_ISR_ISA_COM2:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_COM2

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_COM1:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_COM1

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_LPT2:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_LPT2

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_FloppyDisk:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_FloppyDisk

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_LPT1_Spurious:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_LPT1_Spurious

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_CMOSClock:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_CMOSClock

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_Peripherals0:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_Peripherals0

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_Peripherals1:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_Peripherals1

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_Peripherals2:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_Peripherals2

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_PS2Mouse:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_PS2Mouse

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_FPU:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_FPU

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_PrimaryATAHardDisk:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_PrimaryATAHardDisk

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq

DexprOS_ISR_ISA_SecondaryATAHardDisk:
    /* Push the caller-saved variables on to the stack */
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    call DexprOS_HandleInterrupt_ISA_SecondaryATAHardDisk

    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdx
    popq %rcx
    popq %rax
    iretq


#include "DexprOS/Drivers/PICDriver.h"

#include "DexprOS/Kernel/x86_64/InlineAsm.h"


#define DEXPROS_PIC0_COMMAND_PORT 0x20
#define DEXPROS_PIC0_DATA_PORT 0x21
#define DEXPROS_PIC1_COMMAND_PORT 0xA0
#define DEXPROS_PIC1_DATA_PORT 0xA1

#define DEXPROS_PIC_INITIALIZE_COMMAND 0x11
#define DEXPROS_PIC_EOI_COMMAND 0x20

#define DEXPROS_PIC0_IRQ2 0x4
#define DEXPROS_PIC1_CASCADE_IDENTITY 0x2
#define DEXPROS_PIC_USE_8086_MODE 0x01

#define DEXPROS_IOWAIT_PORT 0x80
#define DEXPROS_IOWAIT_COMMAND 0x0


void DexprOS_InitialisePIC(uint8_t int0Offset, uint8_t int1Offset)
{
    // Initialise the PICs
    // After each command, wait for a few microseconds to give the PICs
    // some time to process them.
    outb(DEXPROS_PIC_INITIALIZE_COMMAND, DEXPROS_PIC0_COMMAND_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);
    outb(DEXPROS_PIC_INITIALIZE_COMMAND, DEXPROS_PIC1_COMMAND_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);

    // Now the PICs are waiting for 3 extra initialization parameters
    // on their data ports.

    // The first one is the IDT entry offset for interrupts.
    outb(int0Offset, DEXPROS_PIC0_DATA_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);
    outb(int1Offset, DEXPROS_PIC1_DATA_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);

    // Tell the master PIC there's a second PIC connected to its IRQ2
    outb(DEXPROS_PIC0_IRQ2, DEXPROS_PIC0_DATA_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);
    // Tell the second PIC its cascade identity
    outb(DEXPROS_PIC1_CASCADE_IDENTITY, DEXPROS_PIC1_DATA_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);

    // Tell the PICs they're working in the 8086 mode
    outb(DEXPROS_PIC_USE_8086_MODE, DEXPROS_PIC0_DATA_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);
    outb(DEXPROS_PIC_USE_8086_MODE, DEXPROS_PIC1_DATA_PORT);
    outb(DEXPROS_IOWAIT_COMMAND, DEXPROS_IOWAIT_PORT);

    // Clear data masks
    outb(0x0, DEXPROS_PIC0_DATA_PORT);
    outb(0x0, DEXPROS_PIC1_DATA_PORT);
}


void DexprOS_PIC_SendEOI(uint8_t irqNumber)
{
    // For IRQs > 7 we need to send EOI to the second PIC too
    if (irqNumber > 7)
        outb(DEXPROS_PIC_EOI_COMMAND, DEXPROS_PIC1_COMMAND_PORT);
    outb(DEXPROS_PIC_EOI_COMMAND, DEXPROS_PIC0_COMMAND_PORT);
}


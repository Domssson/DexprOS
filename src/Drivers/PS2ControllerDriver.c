#include "DexprOS/Drivers/PS2ControllerDriver.h"

#include "DexprOS/Kernel/KernelPanic.h"

#include "DexprOS/Kernel/x86_64/InlineAsm.h"


#define DEXPROS_PS2C_DATA_PORT 0x60
#define DEXPROS_PS2C_COMMAND_PORT 0x64
#define DEXPROS_PS2C_STATUS_PORT 0x64

#define DEXPROS_PS2C_ENABLE_PORT1_COMMAND 0xAE
#define DEXPROS_PS2C_DISABLE_PORT1_COMMAND 0xAD
#define DEXPROS_PS2C_ENABLE_PORT2_COMMAND 0xA8
#define DEXPROS_PS2C_DISABLE_PORT2_COMMAND 0xA7
#define DEXPROS_PS2C_TEST_PORT1_COMMAND 0xAB
#define DEXPROS_PS2C_TEST_PORT2_COMMAND 0xA9

#define DEXPROS_PS2C_SELFTEST_COMMAND 0xAA

#define DEXPROS_PS2C_SELFTEST_PASSED 0x55
#define DEXPROS_PS2C_PORT_TEST_PASSED 0x0

#define DEXPROS_PS2C_READ_CONFIG_BYTE_COMMAND 0x20
#define DEXPROS_PS2C_WRITE_CONFIG_BYTE_COMMAND 0x60

#define DEXPROS_PS2C_DATA_READ_READY_BIT 0x1
#define DEXPROS_PS2C_WRITE_WAIT_BIT 0x2

#define DEXPROS_PS2C_CONFIG_PORT1_INTERRUPT_BIT 0x1
#define DEXPROS_PS2C_CONFIG_PORT2_INTERRUPT_BIT 0x2
#define DEXPROS_PS2C_CONFIG_PORT2_CLOCK_DISABLED_BIT 0x20
#define DEXPROS_PS2C_CONFIG_PORT_TRANSLATION_BIT 0x40


inline static uint8_t ReadConfigurationByte(void)
{
    uint8_t statusByte = 0;

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);
    
    outb(DEXPROS_PS2C_READ_CONFIG_BYTE_COMMAND, DEXPROS_PS2C_COMMAND_PORT);

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_DATA_READ_READY_BIT) != DEXPROS_PS2C_DATA_READ_READY_BIT);

    
    uint8_t configByte = inb(DEXPROS_PS2C_DATA_PORT);
    return configByte;
}

inline static void WriteConfigurationByte(uint8_t value)
{
    uint8_t statusByte = 0;

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);

    outb(DEXPROS_PS2C_WRITE_CONFIG_BYTE_COMMAND, DEXPROS_PS2C_COMMAND_PORT);
    
    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);

    outb(value, DEXPROS_PS2C_DATA_PORT);
}

static void PerformPS2SelfTest(void)
{
    uint8_t statusByte = 0;

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);

    outb(DEXPROS_PS2C_SELFTEST_COMMAND, DEXPROS_PS2C_COMMAND_PORT);

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_DATA_READ_READY_BIT) != DEXPROS_PS2C_DATA_READ_READY_BIT);

    uint8_t selfTestResult = inb(DEXPROS_PS2C_DATA_PORT);

    if (selfTestResult != DEXPROS_PS2C_SELFTEST_PASSED)
    {
        DexprOS_KernelPanic("Boot error: PS/2 controller self-test failed");
    }
}

static void TestPS2Ports(int hasSecondPort,
                         bool* pOutFirstPortWorking,
                         bool* pOutSecondPortWorking)
{
    uint8_t statusByte = 0;

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);

    outb(DEXPROS_PS2C_TEST_PORT1_COMMAND, DEXPROS_PS2C_COMMAND_PORT);

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_DATA_READ_READY_BIT) != DEXPROS_PS2C_DATA_READ_READY_BIT);

    uint8_t portTestResult = inb(DEXPROS_PS2C_DATA_PORT);
    
    if (portTestResult == DEXPROS_PS2C_PORT_TEST_PASSED)
        *pOutFirstPortWorking = true;


    if (hasSecondPort)
    {
        do
        {
            statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
        } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);

        outb(DEXPROS_PS2C_TEST_PORT2_COMMAND, DEXPROS_PS2C_COMMAND_PORT);

        do
        {
            statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
        } while ((statusByte & DEXPROS_PS2C_DATA_READ_READY_BIT) != DEXPROS_PS2C_DATA_READ_READY_BIT);

        portTestResult = inb(DEXPROS_PS2C_DATA_PORT);

        if (portTestResult == DEXPROS_PS2C_PORT_TEST_PASSED)
            *pOutSecondPortWorking = true;
    }
}

inline static void WritePS2Command(uint8_t command)
{
    uint8_t statusByte = 0;

    do
    {
        statusByte = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((statusByte & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);

    outb(command, DEXPROS_PS2C_COMMAND_PORT);
}


DexprOS_PS2ControllerInitResult DexprOS_InitialisePS2Controller(void)
{
    // Disable all devices for them to not mess initialisation
    WritePS2Command(DEXPROS_PS2C_DISABLE_PORT1_COMMAND);
    WritePS2Command(DEXPROS_PS2C_DISABLE_PORT2_COMMAND);

    // Discard the contents of data port by reading it
    inb(DEXPROS_PS2C_DATA_PORT);

    // Retrieve the current configuration byte state
    uint8_t configByte = ReadConfigurationByte();

    // Check whether the PS/2 controller has one or two ports
    bool hasSecondPort = (configByte & DEXPROS_PS2C_CONFIG_PORT2_CLOCK_DISABLED_BIT) == DEXPROS_PS2C_CONFIG_PORT2_CLOCK_DISABLED_BIT;

    // Modify configuration
    configByte &= (~DEXPROS_PS2C_CONFIG_PORT1_INTERRUPT_BIT);
    configByte &= (~DEXPROS_PS2C_CONFIG_PORT2_INTERRUPT_BIT);
    configByte &= (~DEXPROS_PS2C_CONFIG_PORT_TRANSLATION_BIT);

    // Save configuration
    WriteConfigurationByte(configByte);

    PerformPS2SelfTest();

    // Restore configuration as a self test might reset the PS/2 controller
    WriteConfigurationByte(configByte);


    if (hasSecondPort)
    {
        // Query the actual presence of the second channel
        WritePS2Command(DEXPROS_PS2C_ENABLE_PORT2_COMMAND);

        configByte = ReadConfigurationByte();

        if ((configByte & DEXPROS_PS2C_CONFIG_PORT2_CLOCK_DISABLED_BIT) == 0)
        {
            // Disable the second port again
            WritePS2Command(DEXPROS_PS2C_DISABLE_PORT2_COMMAND);
        }
        else
        {
            // It looks like the second port isn't actually working as
            // its clock hasn't switched on.
            hasSecondPort = 0;
        }
    }

    bool firstPortWorking = false;
    bool secondPortWorking = false;

    // Perform tests of the avaible ports
    TestPS2Ports(hasSecondPort, &firstPortWorking, &secondPortWorking);


    if (!firstPortWorking && !secondPortWorking)
    {
        DexprOS_KernelPanic("Boot: couldn't initialise PS/2 controller: no working ports");
    }


    // Finally, enable avaible devices and proper interrupts

    if (firstPortWorking)
        WritePS2Command(DEXPROS_PS2C_ENABLE_PORT1_COMMAND);
    if (secondPortWorking)
        WritePS2Command(DEXPROS_PS2C_ENABLE_PORT2_COMMAND);


    configByte = ReadConfigurationByte();

    if (firstPortWorking)
        configByte |= DEXPROS_PS2C_CONFIG_PORT1_INTERRUPT_BIT;
    if (secondPortWorking)
        configByte |= DEXPROS_PS2C_CONFIG_PORT2_INTERRUPT_BIT;

    WriteConfigurationByte(configByte);


    DexprOS_PS2ControllerInitResult result = {
        .hasKeyboard = firstPortWorking,
        .hasMouse = secondPortWorking
    };
    return result;
}


#include "DexprOS/Drivers/Keyboard/PS2KeyboardDriver.h"

#include "DexprOS/Drivers/Keyboard/PS2KeyboardScanCodes.h"
#include "DexprOS/Drivers/Keyboard/KeyToUnicodeTable.h"

#include "DexprOS/Kernel/KeyboardInput.h"

#include "DexprOS/Kernel/x86_64/InlineAsm.h"

// TODO: create a proper keyboard polling API instead of passing data directly
#include "DexprOS/Shell.h"
extern DexprOS_Shell g_shell;

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define DEXPROS_PS2C_DATA_PORT 0x60
#define DEXPROS_PS2C_STATUS_PORT 0x64

#define DEXPROS_PS2C_WRITE_WAIT_BIT 0x2


#define DEXPROS_PS2K_KEY_DETECTION_ERROR0 0x0
#define DEXPROS_PS2K_KEY_DETECTION_ERROR1 0xFF
#define DEXPROS_PS2K_SELF_TEST_PASSED 0xAA
#define DEXPROS_PS2K_SELF_TEST_FAILED0 0xFC
#define DEXPROS_PS2K_SELF_TEST_FAILED1 0xFD
#define DEXPROS_PS2K_ECHO 0xEE
#define DEXPROS_PS2K_COMMAND_ACKNOWLEDGED 0xFA
#define DEXPROS_PS2K_RESEND 0xFE

#define DEXPROS_PS2K_RESET_COMMAND 0xFF
#define DEXPROS_PS2K_SET_SCAN_CODE_SET_COMMAND 0xF0
#define DEXPROS_PS2K_SET_TYPEMATIC_BYTE_COMMAND 0xF3
#define DEXPROS_PS2K_ENABLE_SCANNING_COMMAND 0xF4

#define DEXPROS_PS2K_REPEAT_DELAY_500MS_RATE_30HZ 0x20

#define DEXPROS_PS2K_SET_LED_STATES_COMMAND 0xED
#define DEXPROS_PS2K_SCROLL_LOCK_LED_BIT 0x1
#define DEXPROS_PS2K_NUMBER_LOCK_LED_BIT 0x2
#define DEXPROS_PS2K_CAPS_LOCK_LED_BIT 0x4


#define DEXPROS_PS2_KEYBOARD_MAX_CMD_BUFFER_SIZE 16
#define DEXPROS_PS2_KEYBOARD_MAX_CMD_RESENDS 3



typedef enum DexprOS_PS2KeyboardListeningMode
{
    PS2K_LISTENING_MODE_TABLE_CHARACTER_LOOKUP = 0,
    PS2K_LISTENING_MODE_BYTE_SEQUENCE
} PS2KeyboardListeningMode;

typedef enum DexprOS_PS2KeyboardCommandState
{
    PS2K_COMMAND_STATE_FREE = 0,
    PS2K_COMMAND_STATE_EXECUTING,
    PS2K_COMMAND_STATE_WAITING_FOR_RESPONSE
} PS2KeyboardCommandState;

struct DexprOS_PS2KeyboardState
{
    uint8_t commandBuffer[DEXPROS_PS2_KEYBOARD_MAX_CMD_BUFFER_SIZE];
    uint8_t commandDataBuffer[DEXPROS_PS2_KEYBOARD_MAX_CMD_BUFFER_SIZE];
    uint8_t commandBufferSize;
    uint8_t numCommandResends;
    PS2KeyboardCommandState currentCommandState;


    PS2KeyboardListeningMode listeningMode;

    const PS2KeyboardScanCodes* pGlobalScanCodeSetTable;
    const PS2KeyboardScanCodes* pCurrentSubScanCodeSetTable;

    const PS2KeyboardSpecialSequence* pCurrentSpecialSequence;
    uint8_t specialSequenceByteIndex;


    DexprOS_KeyState keyStates[DEXPROS_KEY_Max];

    bool capsLockActive;
    bool numLockActive;
    bool scrollLockActive;

    bool capsLockJustActivated;
    bool numLockJustActivated;
    bool scrollLockJustActivated;

    bool anyShiftActive;
    bool anyControlActive;
    bool leftAltActive;
    bool rightAltActive;


    const DexprOS_KeyToUnicodeTable* pUnicodeTable;
};

static struct DexprOS_PS2KeyboardState g_PS2KeyboardState = {0};



static void ExecutePS2KeyboardCommand(void)
{
    g_PS2KeyboardState.currentCommandState = PS2K_COMMAND_STATE_EXECUTING;


    uint8_t command = g_PS2KeyboardState.commandBuffer[0];
    uint8_t data = g_PS2KeyboardState.commandDataBuffer[0];

    // Wait until writing is avaible
    uint8_t controllerStatus = 0;
    do
    {
        controllerStatus = inb(DEXPROS_PS2C_STATUS_PORT);
    } while ((controllerStatus & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);
    
    // Write the command
    outb(command, DEXPROS_PS2C_DATA_PORT);

    // Write the data byte if the command is one of those that consume it
    switch (command)
    {
    case DEXPROS_PS2K_SET_LED_STATES_COMMAND:
    case DEXPROS_PS2K_SET_SCAN_CODE_SET_COMMAND:
    case DEXPROS_PS2K_SET_TYPEMATIC_BYTE_COMMAND:
    case 0xFB:
    case 0xFC:
    case 0xFD:
        do
        {
            controllerStatus = inb(DEXPROS_PS2C_STATUS_PORT);
        } while ((controllerStatus & DEXPROS_PS2C_WRITE_WAIT_BIT) == DEXPROS_PS2C_WRITE_WAIT_BIT);
        
        outb(data, DEXPROS_PS2C_DATA_PORT);
    }


    g_PS2KeyboardState.currentCommandState = PS2K_COMMAND_STATE_WAITING_FOR_RESPONSE;
}


static void PushPS2KeyboardCommand(uint8_t command, uint8_t data)
{
    // Exit early if the command buffer is already full
    if (g_PS2KeyboardState.commandBufferSize == DEXPROS_PS2_KEYBOARD_MAX_CMD_BUFFER_SIZE)
        return;

    uint8_t index = g_PS2KeyboardState.commandBufferSize;
    g_PS2KeyboardState.commandBuffer[index] = command;
    g_PS2KeyboardState.commandDataBuffer[index] = data;
    g_PS2KeyboardState.commandBufferSize += 1;

    if (g_PS2KeyboardState.currentCommandState == PS2K_COMMAND_STATE_FREE)
        ExecutePS2KeyboardCommand();
}

static void NextPS2KeyboardCommand(void)
{
    g_PS2KeyboardState.currentCommandState = PS2K_COMMAND_STATE_FREE;
    g_PS2KeyboardState.numCommandResends = 0;

    for (unsigned i = 1; i < g_PS2KeyboardState.commandBufferSize; ++i)
    {
        g_PS2KeyboardState.commandBuffer[i - 1] = g_PS2KeyboardState.commandBuffer[i];
        g_PS2KeyboardState.commandDataBuffer[i - 1] = g_PS2KeyboardState.commandDataBuffer[i];
    }

    if (g_PS2KeyboardState.commandBufferSize > 0)
        g_PS2KeyboardState.commandBufferSize -= 1;
    
    if (g_PS2KeyboardState.commandBufferSize > 0)
        ExecutePS2KeyboardCommand();
}


inline static void OnPS2KeyboardCommandAcknowledged(void)
{
    if (g_PS2KeyboardState.currentCommandState == PS2K_COMMAND_STATE_WAITING_FOR_RESPONSE)
        NextPS2KeyboardCommand();
}

inline static void ResendPS2KeyboardCommand(void)
{
    if (g_PS2KeyboardState.commandBufferSize > 0 &&
        g_PS2KeyboardState.currentCommandState == PS2K_COMMAND_STATE_WAITING_FOR_RESPONSE)
    {
        if (g_PS2KeyboardState.numCommandResends < DEXPROS_PS2_KEYBOARD_MAX_CMD_RESENDS)
        {
            ExecutePS2KeyboardCommand();
            g_PS2KeyboardState.numCommandResends += 1;
        }
        else
        {
            NextPS2KeyboardCommand();
        }
    }
}

inline static void UpdatePS2KeyboardIndicatorLEDs(void)
{
    uint8_t data = 0;
    if (g_PS2KeyboardState.scrollLockActive)
        data |= DEXPROS_PS2K_SCROLL_LOCK_LED_BIT;
    if (g_PS2KeyboardState.numLockActive)
        data |= DEXPROS_PS2K_NUMBER_LOCK_LED_BIT;
    if (g_PS2KeyboardState.capsLockActive)
        data |= DEXPROS_PS2K_CAPS_LOCK_LED_BIT;

    PushPS2KeyboardCommand(DEXPROS_PS2K_SET_LED_STATES_COMMAND, data);
}


static void InitialisePS2Keyboard(void)
{
    PushPS2KeyboardCommand(DEXPROS_PS2K_SET_SCAN_CODE_SET_COMMAND, 2);
    PushPS2KeyboardCommand(DEXPROS_PS2K_SET_TYPEMATIC_BYTE_COMMAND,
                           DEXPROS_PS2K_REPEAT_DELAY_500MS_RATE_30HZ);
    PushPS2KeyboardCommand(DEXPROS_PS2K_ENABLE_SCANNING_COMMAND, 0);
    UpdatePS2KeyboardIndicatorLEDs();
}



void DexprOS_InitialisePS2KeyboardDriver(const DexprOS_KeyToUnicodeTable* pLayoutTable,
                                         DexprOS_VirtualMemoryAddress rellocOffset)
{
    DexprOS_RellocatePS2KeyboardScanCode2Pointers(rellocOffset);

    g_PS2KeyboardState.commandBufferSize = 0;
    g_PS2KeyboardState.currentCommandState = PS2K_COMMAND_STATE_FREE;
    g_PS2KeyboardState.listeningMode = PS2K_LISTENING_MODE_TABLE_CHARACTER_LOOKUP;
    g_PS2KeyboardState.pGlobalScanCodeSetTable = &g_scanCode2SetGlobal;
    g_PS2KeyboardState.pCurrentSubScanCodeSetTable = &g_scanCode2SetGlobal;
    g_PS2KeyboardState.pCurrentSpecialSequence = NULL;
    g_PS2KeyboardState.specialSequenceByteIndex = 0;

    for (DexprOS_KeyID i = 0; i < DEXPROS_KEY_Max; ++i)
        g_PS2KeyboardState.keyStates[i] = DEXPROS_KEY_STATE_RELEASED;
    
    g_PS2KeyboardState.capsLockActive = false;
    g_PS2KeyboardState.numLockActive = false;
    g_PS2KeyboardState.scrollLockActive = false;
    g_PS2KeyboardState.capsLockJustActivated = false;
    g_PS2KeyboardState.numLockJustActivated = false;
    g_PS2KeyboardState.scrollLockJustActivated = false;

    g_PS2KeyboardState.anyShiftActive = false;
    g_PS2KeyboardState.anyControlActive = false;
    g_PS2KeyboardState.leftAltActive = false;
    g_PS2KeyboardState.rightAltActive = false;

    g_PS2KeyboardState.pUnicodeTable = pLayoutTable;


    PushPS2KeyboardCommand(DEXPROS_PS2K_RESET_COMMAND, 0);
}



typedef enum DexprOS_PS2KeyboardCodeProcessResult
{
    // This value means that an error happened when trying to encode a code
    // from the keyboard. The output key ID is invalid and the processing state 
    // should be reset.
    DEXPROS_PS2K_CODE_FAILURE,
    // Successfully decoded keyboard codes to a key ID. The processing state
    // of the driver should be reset.
    DEXPROS_PS2K_CODE_DECODED_TO_KEY,
    // Successfully interpreted the code as a part of a multi-byte sequence.
    // Although the output key ID is invalid, the keyboard code processing
    // state of the driver should be left untouched.
    DEXPROS_PS2K_CODE_PARTIALLY_DECODED
} PS2KeyboardCodeProcessResult;


static PS2KeyboardCodeProcessResult ProcessPS2KCodeByLookup(uint8_t code,
                                                            DexprOS_KeyID* pOutKeyID,
                                                            PS2KeyboardKeyAction* pOutAction)
{
    const PS2KeyboardScanCodes* pScanCodeTable = g_PS2KeyboardState.pCurrentSubScanCodeSetTable;
    
    // Check if the code is an extender
    for (uint8_t i = 0; i < pScanCodeTable->numByteExtendCodes; ++i)
    {
        if (code == pScanCodeTable->pByteExtendCodes[i])
        {
            g_PS2KeyboardState.pCurrentSubScanCodeSetTable = &pScanCodeTable->pByteExtendedScanCodes[i];
            return DEXPROS_PS2K_CODE_PARTIALLY_DECODED;
        }
    }

    // Check if the code belongs to a special sequence
    for (uint8_t iSeq = 0; iSeq < pScanCodeTable->numSpecialSequences; ++iSeq)
    {
        const PS2KeyboardSpecialSequence* pSequence = &pScanCodeTable->pSpecialSequences[iSeq];
        if (pSequence->sequenceLength > 1)
        {
            if (code == pSequence->pSequence[0])
            {
                g_PS2KeyboardState.listeningMode = PS2K_LISTENING_MODE_BYTE_SEQUENCE;
                g_PS2KeyboardState.pCurrentSpecialSequence = pSequence;
                g_PS2KeyboardState.specialSequenceByteIndex = 1;
                return DEXPROS_PS2K_CODE_PARTIALLY_DECODED;
            }
        }
        else if (pSequence->sequenceLength == 1)
        {
            if (code == pSequence->pSequence[0])
            {
                *pOutKeyID = pSequence->key.keyID;
                *pOutAction = pSequence->key.keyAction;
                goto key_decoded;
            }
        }
    }

    // If the code wasn't a special one, perform a table lookup
    if (code < pScanCodeTable->numKeysPerByte)
    {
        *pOutKeyID = pScanCodeTable->pKeysPerByte[code].keyID;
        *pOutAction = pScanCodeTable->pKeysPerByte[code].keyAction;
    }


    key_decoded:

    if (*pOutKeyID > 0 && *pOutKeyID < DEXPROS_KEY_Max)
        return DEXPROS_PS2K_CODE_DECODED_TO_KEY;
    return DEXPROS_PS2K_CODE_FAILURE;
}

static PS2KeyboardCodeProcessResult ProcessPS2KByteSequenceCode(uint8_t code,
                                                                DexprOS_KeyID* pOutKeyID,
                                                                PS2KeyboardKeyAction* pOutAction)
{
    const PS2KeyboardSpecialSequence* pSequence = g_PS2KeyboardState.pCurrentSpecialSequence;
    const uint8_t seqIndex = g_PS2KeyboardState.specialSequenceByteIndex;
    
    // Make sure the code matches its relevant in the sequence
    if (code != pSequence->pSequence[seqIndex])
    {
        return DEXPROS_PS2K_CODE_FAILURE;
    }

    // Check if we made it to the last code
    if (seqIndex + 1 == pSequence->sequenceLength)
    {
        *pOutKeyID = pSequence->key.keyID;
        *pOutAction = pSequence->key.keyAction;

        if (*pOutKeyID > 0 && *pOutKeyID < DEXPROS_KEY_Max)
            return DEXPROS_PS2K_CODE_DECODED_TO_KEY;
        return DEXPROS_PS2K_CODE_FAILURE;
    }

    // Move to the next byte
    g_PS2KeyboardState.specialSequenceByteIndex += 1;
    return DEXPROS_PS2K_CODE_PARTIALLY_DECODED;
}


static void SwitchPS2KCapsLock(void)
{
    const DexprOS_KeyState state = g_PS2KeyboardState.keyStates[DEXPROS_KEY_CAPS_LOCK];

    if (g_PS2KeyboardState.capsLockActive)
    {
        if (state == DEXPROS_KEY_STATE_RELEASED)
        {
            if (!g_PS2KeyboardState.capsLockJustActivated)
            {
                g_PS2KeyboardState.capsLockActive = false;
                UpdatePS2KeyboardIndicatorLEDs();
            }
            else
            {
                g_PS2KeyboardState.capsLockJustActivated = false;
            }
        }
    }
    else
    {
        if (state == DEXPROS_KEY_STATE_PRESSED)
        {
            g_PS2KeyboardState.capsLockActive = true;
            g_PS2KeyboardState.capsLockJustActivated = true;
            UpdatePS2KeyboardIndicatorLEDs();
        }
    }
}

static void SwitchPS2KNumLock(void)
{
    const DexprOS_KeyState state = g_PS2KeyboardState.keyStates[DEXPROS_KEY_NUM_LOCK];

    if (g_PS2KeyboardState.numLockActive)
    {
        if (state == DEXPROS_KEY_STATE_RELEASED)
        {
            if (!g_PS2KeyboardState.numLockJustActivated)
            {
                g_PS2KeyboardState.numLockActive = false;
                UpdatePS2KeyboardIndicatorLEDs();
            }
            else
            {
                g_PS2KeyboardState.numLockJustActivated = false;
            }
        }
    }
    else
    {
        if (state == DEXPROS_KEY_STATE_PRESSED)
        {
            g_PS2KeyboardState.numLockActive = true;
            g_PS2KeyboardState.numLockJustActivated = true;
            UpdatePS2KeyboardIndicatorLEDs();
        }
    }
}

static void SwitchPS2KScrollLock(void)
{
    const DexprOS_KeyState state = g_PS2KeyboardState.keyStates[DEXPROS_KEY_SCROLL_LOCK];

    if (g_PS2KeyboardState.scrollLockActive)
    {
        if (state == DEXPROS_KEY_STATE_RELEASED)
        {
            if (!g_PS2KeyboardState.scrollLockJustActivated)
            {
                g_PS2KeyboardState.scrollLockActive = false;
                UpdatePS2KeyboardIndicatorLEDs();
            }
            else
            {
                g_PS2KeyboardState.scrollLockJustActivated = false;
            }
        }
    }
    else
    {
        if (state == DEXPROS_KEY_STATE_PRESSED)
        {
            g_PS2KeyboardState.scrollLockActive = true;
            g_PS2KeyboardState.scrollLockJustActivated = true;
            UpdatePS2KeyboardIndicatorLEDs();
        }
    }
}


static void UpdatePS2KeyboardKeyState(DexprOS_KeyID keyID,
                                      PS2KeyboardKeyAction action)
{
    if (action == DEXPROS_PS2KEYBOARD_KEY_ACTION_PRESSED)
    {
        if (g_PS2KeyboardState.keyStates[keyID] == DEXPROS_KEY_STATE_PRESSED)
            g_PS2KeyboardState.keyStates[keyID] = DEXPROS_KEY_STATE_REPEAT;
        else if (g_PS2KeyboardState.keyStates[keyID] == DEXPROS_KEY_STATE_RELEASED)
            g_PS2KeyboardState.keyStates[keyID] = DEXPROS_KEY_STATE_PRESSED;
    }
    else if (action == DEXPROS_PS2KEYBOARD_KEY_ACTION_RELEASED)
    {
        g_PS2KeyboardState.keyStates[keyID] = DEXPROS_KEY_STATE_RELEASED;
    }
    else if (action == DEXPROS_PS2KEYBOARD_KEY_ACTION_PRESSED_RELEASED)
    {
        g_PS2KeyboardState.keyStates[keyID] = DEXPROS_KEY_STATE_RELEASED;
    }
}

static void UpdatePS2KeyboardModifierState(DexprOS_KeyID keyID)
{
    switch (keyID)
    {
    case DEXPROS_KEY_CAPS_LOCK:
        SwitchPS2KCapsLock();
        break;
    case DEXPROS_KEY_NUM_LOCK:
        SwitchPS2KNumLock();
        break;
    case DEXPROS_KEY_SCROLL_LOCK:
        SwitchPS2KScrollLock();
        break;

    case DEXPROS_KEY_LEFT_SHIFT:
    case DEXPROS_KEY_RIGHT_SHIFT:
        {
            DexprOS_KeyState left = g_PS2KeyboardState.keyStates[DEXPROS_KEY_LEFT_SHIFT];
            DexprOS_KeyState right = g_PS2KeyboardState.keyStates[DEXPROS_KEY_RIGHT_SHIFT];
            g_PS2KeyboardState.anyShiftActive = (left != DEXPROS_KEY_STATE_RELEASED ||
                                                 right != DEXPROS_KEY_STATE_RELEASED);
        }
        break;
    case DEXPROS_KEY_LEFT_CONTROL:
    case DEXPROS_KEY_RIGHT_CONTROL:
        {
            DexprOS_KeyState left = g_PS2KeyboardState.keyStates[DEXPROS_KEY_LEFT_CONTROL];
            DexprOS_KeyState right = g_PS2KeyboardState.keyStates[DEXPROS_KEY_RIGHT_CONTROL];
            g_PS2KeyboardState.anyControlActive = (left != DEXPROS_KEY_STATE_RELEASED ||
                                                   right != DEXPROS_KEY_STATE_RELEASED);
        }
        break;
    case DEXPROS_KEY_LEFT_ALT:
        g_PS2KeyboardState.leftAltActive = (g_PS2KeyboardState.keyStates[DEXPROS_KEY_LEFT_ALT] !=
                                            DEXPROS_KEY_STATE_RELEASED);
        break;
    case DEXPROS_KEY_RIGHT_ALT:
        g_PS2KeyboardState.rightAltActive = (g_PS2KeyboardState.keyStates[DEXPROS_KEY_RIGHT_ALT] !=
                                             DEXPROS_KEY_STATE_RELEASED);
        break;
    
    default:
        break;
    }
}


static void ProcessKeyToUnicodeCharacter(DexprOS_KeyID key)
{
    if (g_PS2KeyboardState.pUnicodeTable == NULL)
        return;
    if (key == 0 || key > DEXPROS_KEY_Max)
        return;

    
    if (g_PS2KeyboardState.keyStates[key] == DEXPROS_KEY_STATE_PRESSED ||
        g_PS2KeyboardState.keyStates[key] == DEXPROS_KEY_STATE_REPEAT)
    {
        DexprOS_UnicodeCodepoint codepoint = 0;

        DexprOS_KeyToUnicodeEntry keyEntry = g_PS2KeyboardState.pUnicodeTable->table[key];

        switch (keyEntry.conversionType)
        {
        case DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_INVALID:
            return;
        
        case DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_AFFECTED:
            {
                if (g_PS2KeyboardState.anyControlActive || g_PS2KeyboardState.leftAltActive)
                    return;

                const bool uppercase = g_PS2KeyboardState.anyShiftActive ^ g_PS2KeyboardState.capsLockActive;

                if (uppercase)
                {
                    if (g_PS2KeyboardState.rightAltActive)
                        codepoint = keyEntry.altGrUpperCaseCode;
                    else
                        codepoint = keyEntry.upperCaseCode;
                }
                else
                {
                    if (g_PS2KeyboardState.rightAltActive)
                        codepoint = keyEntry.altGrCode;
                    else
                        codepoint = keyEntry.standardCode;
                }
            }
            break;

        case DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_CAPS_LOCK_UNAFFECTED:
            if (g_PS2KeyboardState.anyControlActive || g_PS2KeyboardState.leftAltActive)
                return;

            if (g_PS2KeyboardState.anyShiftActive)
            {
                if (g_PS2KeyboardState.rightAltActive)
                    codepoint = keyEntry.altGrUpperCaseCode;
                else
                    codepoint = keyEntry.upperCaseCode;
            }
            else
            {
                if (g_PS2KeyboardState.rightAltActive)
                    codepoint = keyEntry.altGrCode;
                else
                    codepoint = keyEntry.standardCode;
            }
            break;

        case DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_NUMBERPAD:
            if (g_PS2KeyboardState.anyControlActive || g_PS2KeyboardState.leftAltActive)
                return;
            if (g_PS2KeyboardState.numLockActive)
                codepoint = keyEntry.standardCode;
            break;

        case DEXPROS_KEY_TO_UNICODE_CONVERSION_TYPE_ENTER:
            codepoint = keyEntry.standardCode;
            break;
        }

        if (codepoint != 0)
        {
            DexprOS_ShellKeyboardCharacterEvent(&g_shell, codepoint);
        }
    }
}


void DexprOS_ProcessPS2KeyboardInterrupt(void)
{
    // Read the signal from the keyboard
    uint8_t code = inb(DEXPROS_PS2C_DATA_PORT);

    switch (code)
    {
        case DEXPROS_PS2K_KEY_DETECTION_ERROR0:
        case DEXPROS_PS2K_KEY_DETECTION_ERROR1:
            return;

        case DEXPROS_PS2K_SELF_TEST_PASSED:
            InitialisePS2Keyboard();
            return;

        case DEXPROS_PS2K_SELF_TEST_FAILED0:
        case DEXPROS_PS2K_SELF_TEST_FAILED1:
            InitialisePS2Keyboard();
            return;

        case DEXPROS_PS2K_ECHO:
            return;

        case DEXPROS_PS2K_COMMAND_ACKNOWLEDGED:
            OnPS2KeyboardCommandAcknowledged();
            return;
        case DEXPROS_PS2K_RESEND:
            ResendPS2KeyboardCommand();
            return;
    }


    DexprOS_KeyID keyID = 0;
    PS2KeyboardKeyAction action = {0};


    PS2KeyboardCodeProcessResult result = {0};

    if (g_PS2KeyboardState.listeningMode == PS2K_LISTENING_MODE_TABLE_CHARACTER_LOOKUP)
    {
        result = ProcessPS2KCodeByLookup(code, &keyID, &action);
    }
    else if (g_PS2KeyboardState.listeningMode == PS2K_LISTENING_MODE_BYTE_SEQUENCE)
    {
        result = ProcessPS2KByteSequenceCode(code, &keyID, &action);
    }

    if (result == DEXPROS_PS2K_CODE_DECODED_TO_KEY ||
        result == DEXPROS_PS2K_CODE_FAILURE)
    {
        // After fully mapping the appriopriate code, restore the mode and set
        // the current conversion table back to the global one
        g_PS2KeyboardState.listeningMode = PS2K_LISTENING_MODE_TABLE_CHARACTER_LOOKUP;
        g_PS2KeyboardState.pCurrentSubScanCodeSetTable = g_PS2KeyboardState.pGlobalScanCodeSetTable;
    }

    if (result == DEXPROS_PS2K_CODE_FAILURE)
        return;


    UpdatePS2KeyboardKeyState(keyID, action);
    UpdatePS2KeyboardModifierState(keyID);


    DexprOS_ShellKeyboardKeyEvent(&g_shell,
                                  keyID,
                                  g_PS2KeyboardState.keyStates[keyID]);


    ProcessKeyToUnicodeCharacter(keyID);
}

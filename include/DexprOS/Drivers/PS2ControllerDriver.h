#ifndef DEXPROS_DRIVERS_PS2CONTROLLERDRIVER_H_INCLUDED
#define DEXPROS_DRIVERS_PS2CONTROLLERDRIVER_H_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct DexprOS_PS2ControllerInitResult
{
    bool hasKeyboard;
    bool hasMouse;
} DexprOS_PS2ControllerInitResult;


DexprOS_PS2ControllerInitResult DexprOS_InitialisePS2Controller(void);


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_DRIVERS_PS2CONTROLLERDRIVER_H_INCLUDED

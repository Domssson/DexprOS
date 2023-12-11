#ifndef DEXPROS_DRIVERS_GRAPHICS_CPUGRAPHICSDRIVER_H_INCLUDED
#define DEXPROS_DRIVERS_GRAPHICS_CPUGRAPHICSDRIVER_H_INCLUDED

#include "GraphicsDriver.h"

#include <efi.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum DexprOS_CpuGraphicsDrvInitError
{
    DEXPROS_CPU_GRAPHICS_DRV_INIT_SUCCESS = 0,
    DEXPROS_CPU_GRAPHICS_DRV_INIT_MEMORY_ERROR,
    DEXPROS_CPU_GRAPHICS_DRV_INIT_UNSUPPORTED_FORMAT,
    DEXPROS_CPU_GRAPHICS_DRV_INIT_Max
} DexprOS_CpuGraphicsDrvInitError;


DexprOS_CpuGraphicsDrvInitError DexprOS_InitCpuGraphicsDriver(DexprOS_GraphicsDriver* pDriver,
                                                              EFI_GRAPHICS_OUTPUT_PROTOCOL* pGop,
                                                              EFI_BOOT_SERVICES* pBootServices);

void DexprOS_DestroyCpuGraphicsDriver(DexprOS_GraphicsDriver* pDriver);


#ifdef __cplusplus
}
#endif


#endif // DEXPROS_DRIVERS_GRAPHICS_CPUGRAPHICSDRIVER_H_INCLUDED


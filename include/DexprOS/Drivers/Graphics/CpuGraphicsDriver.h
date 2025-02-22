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


typedef enum DexprOS_CpuGrSwapBufferOp
{
    DEXPROS_CPU_GR_SWAP_BUFFER_NONE_OP = 0,
    DEXPROS_CPU_GR_SWAP_BUFFER_FULL_OP,
    DEXPROS_CPU_GR_SWAP_BUFFER_REGION_OP
} DexprOS_CpuGrSwapBufferOp;

typedef struct DexprOS_CpuGrSwapBufferInfo
{
    DexprOS_CpuGrSwapBufferOp swapOp;
    uint32_t regionStartX;
    uint32_t regionStartY;
    uint32_t regionWidth;
    uint32_t regionHeight;
} DexprOS_CpuGrSwapBufferInfo;


typedef struct DexprOS_CpuGrGraphicsDrvData
{
    uint32_t presentationWidth;
    uint32_t presentationHeight;
    uint32_t presentationPixelBytes;
    uint32_t presentationPixelStride;

    void* pMainFramebufferMemory;
    size_t mainFramebufferSize;

    void* pRenderFramebufferMemory;

    DexprOS_GrFramebufferFormat framebufferFormat;

    DexprOS_CpuGrSwapBufferInfo swapBufferInfo;
} DexprOS_CpuGrGraphicsDrvData;



DexprOS_CpuGraphicsDrvInitError DexprOS_InitCpuGraphicsDriver(DexprOS_GraphicsDriver* pDriver,
                                                              DexprOS_CpuGrGraphicsDrvData* pDrvData,
                                                              const DexprOS_StartupFramebufferInfo* pFbInfo,
                                                              DexprOS_VirtualMemoryAddress rellocOffset);

void DexprOS_DestroyCpuGraphicsDriver(DexprOS_GraphicsDriver* pDriver);


#ifdef __cplusplus
}
#endif


#endif // DEXPROS_DRIVERS_GRAPHICS_CPUGRAPHICSDRIVER_H_INCLUDED


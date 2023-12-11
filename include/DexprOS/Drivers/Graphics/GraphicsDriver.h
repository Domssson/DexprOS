#ifndef DEXPROS_DRIVERS_GRAPHICS_GRAPHICSDRIVER_H_INCLUDED
#define DEXPROS_DRIVERS_GRAPHICS_GRAPHICSDRIVER_H_INCLUDED

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef uint8_t DexprOS_GrColorValue;


typedef enum DexprOS_GrImageFormat{
    DEXPROS_GR_IMAGE_FORMAT_RGB8,
    DEXPROS_GR_IMAGE_FORMAT_RGBA8,
    DEXPROS_GR_IMAGE_FORMAT_Max
} DexprOS_GrImageFormat;


typedef struct DexprOS_GrGraphicsColor
{
    DexprOS_GrColorValue r;
    DexprOS_GrColorValue g;
    DexprOS_GrColorValue b;
} DexprOS_GrGraphicsColor;


typedef struct DexprOS_GrImageRegion
{
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} DexprOS_GrImageRegion;


typedef void(*DexprOS_GrSwapBuffers)(void* pDrvData);

typedef void(*DexprOS_GrClearScreen)(void* pDrvData, DexprOS_GrGraphicsColor color);

typedef void(*DexprOS_GrClearScreenRegions)(void* pDrvData,
                                              size_t numRegions,
                                              const DexprOS_GrImageRegion* pRegions,
                                              DexprOS_GrGraphicsColor color);

typedef void(*DexprOS_GrBlitImageToFramebuffer)(void* pDrvData,
                                                  const void* pImage,
                                                  DexprOS_GrImageFormat imageFormat,
                                                  int32_t imageX, int32_t imageY,
                                                  uint32_t imageRowStride,
                                                  int32_t fbX, int32_t fbY,
                                                  int32_t width, int32_t height);


typedef struct DexprOS_GraphicsDriver
{
    DexprOS_GrSwapBuffers swapBuffers;

    DexprOS_GrClearScreen clearScreen;
    DexprOS_GrClearScreenRegions clearScreenRegions;

    DexprOS_GrBlitImageToFramebuffer blitImageToFramebuffer;
    

    void* pDriverData;
} DexprOS_GraphicsDriver;


#ifdef __cplusplus
}
#endif

#endif // DEXPROS_DRIVERS_GRAPHICS_GRAPHICSDRIVER_H_INCLUDED


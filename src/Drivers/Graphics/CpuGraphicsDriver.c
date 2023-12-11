#include "DexprOS/Drivers/Graphics/CpuGraphicsDriver.h"

#include <stdint.h>
#include <stddef.h>


typedef enum DexprOS_CpuGrFramebufferFormat
{
    DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_RGB8_RESERVED8,
    DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_BGR8_RESERVED8,
    DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_Max
} DexprOS_CpuGrFramebufferFormat;

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

    DexprOS_CpuGrFramebufferFormat framebufferFormat;

    DexprOS_CpuGrSwapBufferInfo swapBufferInfo;
} DexprOS_CpuGrGraphicsDrvData;



static void* CpuGrGraphicsMemcpy(void* dest, const void* source, size_t count)
{
    uintptr_t destValue = (uintptr_t)dest;
    uintptr_t sourceValue = (uintptr_t)source;
    
    if ((destValue % 8) == (sourceValue % 8) && count >= 8)
    {
        uint8_t numAlignBytes = (destValue % 8);

        uint8_t* pAlignDst = (uint8_t*)dest;
        const uint8_t* pAlignSrc = (const uint8_t*)source;
        const uint8_t* pAlignSrcEnd = pAlignSrc + numAlignBytes;
        for (; pAlignSrc != pAlignSrcEnd; ++pAlignSrc, ++pAlignDst)
        {
            *pAlignDst = *pAlignSrc;
        }

        uint64_t* pDst = (uint64_t*)pAlignDst;
        const uint64_t* pSrc = (const uint64_t*)pAlignSrc;
        const uint64_t* pSrcEnd = pSrc + (count - numAlignBytes) / 8;
        for (; pSrc != pSrcEnd; ++pSrc, ++pDst)
        {
            *pDst = *pSrc;
        }

        size_t numRemBytes = ((count - numAlignBytes) % 8);
        uint8_t* pRemDst = (uint8_t*)pDst;
        const uint8_t* pRemSrc = (const uint8_t*)pSrc;
        const uint8_t* pRemSrcEnd = pRemSrc + numRemBytes;
        for (; pRemSrc != pRemSrcEnd; ++pRemSrc, ++pRemDst)
        {
            *pRemDst = *pRemSrc;
        }
    }
    else if ((destValue % 4) == (sourceValue % 4) && count >= 4)
    {
        uint8_t numAlignBytes = (destValue % 4);

        uint8_t* pAlignDst = (uint8_t*)dest;
        const uint8_t* pAlignSrc = (const uint8_t*)source;
        const uint8_t* pAlignSrcEnd = pAlignSrc + numAlignBytes;
        for (; pAlignSrc != pAlignSrcEnd; ++pAlignSrc, ++pAlignDst)
        {
            *pAlignDst = *pAlignSrc;
        }

        uint32_t* pDst = (uint32_t*)pAlignDst;
        const uint32_t* pSrc = (const uint32_t*)pAlignSrc;
        const uint32_t* pSrcEnd = pSrc + (count - numAlignBytes) / 4;
        for (; pSrc != pSrcEnd; ++pSrc, ++pDst)
        {
            *pDst = *pSrc;
        }

        size_t numRemBytes = ((count - numAlignBytes) % 4);
        uint8_t* pRemDst = (uint8_t*)pDst;
        const uint8_t* pRemSrc = (const uint8_t*)pSrc;
        const uint8_t* pRemSrcEnd = pRemSrc + numRemBytes;
        for (; pRemSrc != pRemSrcEnd; ++pRemSrc, ++pRemDst)
        {
            *pRemDst = *pRemSrc;
        }
    }
    else if ((destValue % 2) == (sourceValue % 2) && count >= 2)
    {
        uint8_t numAlignBytes = (destValue % 2);

        uint8_t* pAlignDst = (uint8_t*)dest;
        const uint8_t* pAlignSrc = (const uint8_t*)source;
        const uint8_t* pAlignSrcEnd = pAlignSrc + numAlignBytes;
        for (; pAlignSrc != pAlignSrcEnd; ++pAlignSrc, ++pAlignDst)
        {
            *pAlignDst = *pAlignSrc;
        }

        uint16_t* pDst = (uint16_t*)pAlignDst;
        const uint16_t* pSrc = (const uint16_t*)pAlignSrc;
        const uint16_t* pSrcEnd = pSrc + (count - numAlignBytes) / 2;
        for (; pSrc != pSrcEnd; ++pSrc, ++pDst)
        {
            *pDst = *pSrc;
        }

        size_t numRemBytes = ((count - numAlignBytes) % 2);
        uint8_t* pRemDst = (uint8_t*)pDst;
        const uint8_t* pRemSrc = (const uint8_t*)pSrc;
        const uint8_t* pRemSrcEnd = pRemSrc + numRemBytes;
        for (; pRemSrc != pRemSrcEnd; ++pRemSrc, ++pRemDst)
        {
            *pRemDst = *pRemSrc;
        }
    }
    else
    {
        unsigned char* pDst = (unsigned char*)dest;
        const unsigned char* pSrc = (const unsigned char*)source;
        const unsigned char* pSrcEnd = pSrc + count;

        for (; pSrc != pSrcEnd; ++pSrc, ++pDst)
        {
            *pDst = *pSrc;
        }
    }

    return dest;
}



static void CpuGrMarkScreenRegionForSwapping(DexprOS_CpuGrGraphicsDrvData* pDrv,
                                             uint32_t x,
                                             uint32_t y,
                                             uint32_t width,
                                             uint32_t height)
{
    // Change the screen swap buffer region
    
    switch (pDrv->swapBufferInfo.swapOp)
    {
    case DEXPROS_CPU_GR_SWAP_BUFFER_FULL_OP:
        return;

    case DEXPROS_CPU_GR_SWAP_BUFFER_REGION_OP:
        {
            uint32_t oldStartX = pDrv->swapBufferInfo.regionStartX;
            uint32_t oldStartY = pDrv->swapBufferInfo.regionStartY;
            uint32_t oldEndX = oldStartX + pDrv->swapBufferInfo.regionWidth;
            uint32_t oldEndY = oldStartY + pDrv->swapBufferInfo.regionHeight;

            uint32_t startX = x;
            uint32_t startY = y;
            uint32_t endX = x + width;
            uint32_t endY = y + height;

            uint32_t newStartX = oldStartX < startX ? oldStartX : startX;
            uint32_t newStartY = oldStartY < startY ? oldStartY : startY;
            uint32_t newEndX = oldEndX > endX ? oldEndX : endX;
            uint32_t newEndY = oldEndY > endY ? oldEndY : endY;

            pDrv->swapBufferInfo.regionStartX = newStartX;
            pDrv->swapBufferInfo.regionStartY = newStartY;
            pDrv->swapBufferInfo.regionWidth = newEndX - newStartX;
            pDrv->swapBufferInfo.regionHeight = newEndY - newStartY;
            return;
        }
    
    case DEXPROS_CPU_GR_SWAP_BUFFER_NONE_OP:
        pDrv->swapBufferInfo.regionStartX = x;
        pDrv->swapBufferInfo.regionStartY = y;
        pDrv->swapBufferInfo.regionWidth = width;
        pDrv->swapBufferInfo.regionHeight = height;
        pDrv->swapBufferInfo.swapOp = DEXPROS_CPU_GR_SWAP_BUFFER_REGION_OP;
        return;
    }
}


static void CpuGrSwapBuffers(void* pDrvData)
{
    DexprOS_CpuGrGraphicsDrvData* pData = (DexprOS_CpuGrGraphicsDrvData*)pDrvData;

    switch (pData->swapBufferInfo.swapOp)
    {
    case DEXPROS_CPU_GR_SWAP_BUFFER_FULL_OP:
        {
            CpuGrGraphicsMemcpy(pData->pMainFramebufferMemory,
                                  pData->pRenderFramebufferMemory,
                                  pData->mainFramebufferSize);
        }
        break;
    
    case DEXPROS_CPU_GR_SWAP_BUFFER_REGION_OP:
        {
            const size_t rowBytes = pData->presentationPixelStride *
                                    pData->presentationPixelBytes;

            const size_t regionRowBytes = pData->swapBufferInfo.regionWidth *
                                          pData->presentationPixelBytes;

            const size_t offset = pData->swapBufferInfo.regionStartY *
                                  rowBytes +
                                  pData->swapBufferInfo.regionStartX *
                                  pData->presentationPixelBytes;
            

            const unsigned char* pSrc = (const unsigned char*)pData->pRenderFramebufferMemory +
                                        offset;
            unsigned char* pDst = (unsigned char*)pData->pMainFramebufferMemory + offset;

            for (uint32_t h = 0; h < pData->swapBufferInfo.regionHeight; ++h)
            {
                unsigned char* pRowDst = pDst + h * rowBytes;
                const unsigned char* pRowSrc = pSrc + h * rowBytes;
                CpuGrGraphicsMemcpy(pRowDst, pRowSrc, regionRowBytes);
            }
        }
        break;

    case DEXPROS_CPU_GR_SWAP_BUFFER_NONE_OP:
        break;
    }


    pData->swapBufferInfo.swapOp = DEXPROS_CPU_GR_SWAP_BUFFER_NONE_OP;
}


static void CpuGrClearScreenRGB(DexprOS_CpuGrGraphicsDrvData* pDrvData,
                                DexprOS_GrColorValue r,
                                DexprOS_GrColorValue g,
                                DexprOS_GrColorValue b)
{
    const uint32_t byteStride = pDrvData->presentationPixelStride *
                                pDrvData->presentationPixelBytes;
    const uint32_t bytesPerPixel = pDrvData->presentationPixelBytes;


    for (uint32_t h = 0; h < pDrvData->presentationHeight; ++h)
    {
        uint8_t* pRow = ((uint8_t*)pDrvData->pRenderFramebufferMemory) + h * byteStride;

        for (uint32_t w = 0; w < pDrvData->presentationWidth; ++w)
        {
            uint8_t* pPixel = pRow + w * bytesPerPixel;
            pPixel[0] = r;
            pPixel[1] = g;
            pPixel[2] = b;
            pPixel[3] = 0;
        }
    }
}

static void CpuGrClearScreenBGR(DexprOS_CpuGrGraphicsDrvData* pDrvData,
                                DexprOS_GrColorValue r,
                                DexprOS_GrColorValue g,
                                DexprOS_GrColorValue b)
{
    const uint32_t byteStride = pDrvData->presentationPixelStride *
                                pDrvData->presentationPixelBytes;
    const uint32_t bytesPerPixel = pDrvData->presentationPixelBytes;


    for (uint32_t h = 0; h < pDrvData->presentationHeight; ++h)
    {
        uint8_t* pRow = ((uint8_t*)pDrvData->pRenderFramebufferMemory) + h * byteStride;

        for (uint32_t w = 0; w < pDrvData->presentationWidth; ++w)
        {
            uint8_t* pPixel = pRow + w * bytesPerPixel;
            pPixel[0] = b;
            pPixel[1] = g;
            pPixel[2] = r;
            pPixel[3] = 0;
        }
    }
}

static void CpuGrGraphicsClearScreen(void* pDrvData, DexprOS_GrGraphicsColor color)
{
    DexprOS_CpuGrGraphicsDrvData* pData = (DexprOS_CpuGrGraphicsDrvData*)pDrvData;

    pData->swapBufferInfo.swapOp = DEXPROS_CPU_GR_SWAP_BUFFER_FULL_OP;

    switch (pData->framebufferFormat)
    {
    case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_RGB8_RESERVED8:
        CpuGrClearScreenRGB(pData, color.r, color.g, color.b);
        return;
    case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_BGR8_RESERVED8:
        CpuGrClearScreenBGR(pData, color.r, color.g, color.b);
        return;
    case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_Max:
        return;
    }
}


static void CpuGrClearScreenRegionRGB(DexprOS_CpuGrGraphicsDrvData* pDrvData,
                                        uint32_t x,
                                        uint32_t y,
                                        uint32_t width,
                                        uint32_t height,
                                        DexprOS_GrColorValue r,
                                        DexprOS_GrColorValue g,
                                        DexprOS_GrColorValue b)
{
    const uint32_t byteStride = pDrvData->presentationPixelStride *
                                pDrvData->presentationPixelBytes;
    const uint32_t bytesPerPixel = pDrvData->presentationPixelBytes;


    uint8_t* pRegionOffset = (uint8_t*)pDrvData->pRenderFramebufferMemory +
                             y * byteStride + x * bytesPerPixel;

    for (uint32_t h = 0; h < height; ++h)
    {
        uint8_t* pRow = pRegionOffset + h * byteStride;

        for (uint32_t w = 0; w < width; ++w)
        {
            uint8_t* pPixel = pRow + w * bytesPerPixel;
            pPixel[0] = r;
            pPixel[1] = g;
            pPixel[2] = b;
            pPixel[3] = 0;
        }
    }
}

static void CpuGrClearScreenRegionBGR(DexprOS_CpuGrGraphicsDrvData* pDrvData,
                                      uint32_t x,
                                      uint32_t y,
                                      uint32_t width,
                                      uint32_t height,
                                      DexprOS_GrColorValue r,
                                      DexprOS_GrColorValue g,
                                      DexprOS_GrColorValue b)
{
    const uint32_t byteStride = pDrvData->presentationPixelStride *
                                pDrvData->presentationPixelBytes;
    const uint32_t bytesPerPixel = pDrvData->presentationPixelBytes;


    uint8_t* pRegionOffset = (uint8_t*)pDrvData->pRenderFramebufferMemory +
                             y * byteStride + x * bytesPerPixel;

    for (uint32_t h = 0; h < height; ++h)
    {
        uint8_t* pRow = pRegionOffset + h * byteStride;

        for (uint32_t w = 0; w < width; ++w)
        {
            uint8_t* pPixel = pRow + w * bytesPerPixel;
            pPixel[0] = b;
            pPixel[1] = g;
            pPixel[2] = r;
            pPixel[3] = 0;
        }
    }
}

static void CpuGrGraphicsClearScreenRegions(void* pDrvData,
                                            size_t numRegions,
                                            const DexprOS_GrImageRegion* pRegions,
                                            DexprOS_GrGraphicsColor color)
{
    DexprOS_CpuGrGraphicsDrvData* pDrv = (DexprOS_CpuGrGraphicsDrvData*)pDrvData;

    
    for (size_t i = 0; i < numRegions; ++i)
    {
        uint32_t startX = (pRegions[i].x >= 0 ? (uint32_t)pRegions[i].x : 0);
        uint32_t startY = (pRegions[i].y >= 0 ? (uint32_t)pRegions[i].y : 0);
        uint32_t width = pRegions->width;
        uint32_t height = pRegions->height;

        if (startX >= pDrv->presentationWidth ||
            startY >= pDrv->presentationHeight)
            continue;

        if (pRegions[i].x < 0)
        {
            if ((uint32_t)(-pRegions[i].x) >= width)
                continue;
            width -= (uint32_t)(-pRegions[i].x);
        }
        if (pRegions[i].y < 0)
        {
            if ((uint32_t)(-pRegions[i].y) >= height)
                continue;
            height -= (uint32_t)(-pRegions[i].y);
        }

        if (startX + width > pDrv->presentationWidth)
            width = pDrv->presentationWidth - startX;
        if (startY + height > pDrv->presentationHeight)
            height = pDrv->presentationHeight - startY;


        CpuGrMarkScreenRegionForSwapping(pDrv,
                                           startX, startY,
                                           width, height);


        switch (pDrv->framebufferFormat)
        {
        case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_RGB8_RESERVED8:
            CpuGrClearScreenRegionRGB(pDrv,
                                        startX, startY,
                                        width, height,
                                        color.r, color.g, color.b);
            break;
        case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_BGR8_RESERVED8:
            CpuGrClearScreenRegionBGR(pDrv,
                                        startX, startY,
                                        width, height,
                                        color.r, color.g, color.b);
            break;
        case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_Max:
            break;
        }
    }
}



static void CpuGrGraphicsBlitRGB8ToRGB8R(const void* pSrcImage,
                                         void* pDstImage,
                                         int32_t imageX, int32_t imageY,
                                         uint32_t imageRowStride,
                                         int32_t fbX, int32_t fbY,
                                         uint32_t fbRowStride,
                                         int32_t width, int32_t height)
{
    const uint8_t* pSrcBase = (const uint8_t*)pSrcImage +
                              imageY * imageRowStride + imageX * 3;
    uint8_t* pDstBase = (uint8_t*)pDstImage +
                        fbY * fbRowStride + fbX * 4;


    for (int32_t h = 0; h < height; ++h)
    {
        const uint8_t* pSrcRow = pSrcBase + h * imageRowStride;
        uint8_t* pDstRow = pDstBase + h * fbRowStride;

        for (int32_t w = 0; w < width; ++w)
        {
            const uint8_t* pSrcPixel = pSrcRow + w * 3;
            uint8_t* pDstPixel = pDstRow + w * 4;

            pDstPixel[0] = pSrcPixel[0];
            pDstPixel[1] = pSrcPixel[1];
            pDstPixel[2] = pSrcPixel[2];
        }
    }
}
static void CpuGrGraphicsBlitRGBA8ToRGB8R(const void* pSrcImage,
                                          void* pDstImage,
                                          int32_t imageX, int32_t imageY,
                                          uint32_t imageRowStride,
                                          int32_t fbX, int32_t fbY,
                                          uint32_t fbRowStride,
                                          int32_t width, int32_t height)
{
    const uint8_t* pSrcBase = (const uint8_t*)pSrcImage +
                              imageY * imageRowStride + imageX * 4;
    uint8_t* pDstBase = (uint8_t*)pDstImage +
                        fbY * fbRowStride + fbX * 4;


    for (int32_t h = 0; h < height; ++h)
    {
        const uint8_t* pSrcRow = pSrcBase + h * imageRowStride;
        uint8_t* pDstRow = pDstBase + h * fbRowStride;

        for (int32_t w = 0; w < width; ++w)
        {
            const uint8_t* pSrcPixel = pSrcRow + w * 4;
            uint8_t* pDstPixel = pDstRow + w * 4;


            const double red = (double)pSrcPixel[0];
            const double green = (double)pSrcPixel[1];
            const double blue = (double)pSrcPixel[2];
            const double alpha = (double)pSrcPixel[3] / 255.0;

            const double alpha_m1 = 1.0 - alpha;

            pDstPixel[0] = (uint8_t)(red * alpha + (double)pDstPixel[0] * alpha_m1);
            pDstPixel[1] = (uint8_t)(green * alpha + (double)pDstPixel[1] * alpha_m1);
            pDstPixel[2] = (uint8_t)(blue * alpha + (double)pDstPixel[2] * alpha_m1);
        }
    }
}

static void CpuGrGraphicsBlitRGB8ToBGR8R(const void* pSrcImage,
                                         void* pDstImage,
                                         int32_t imageX, int32_t imageY,
                                         uint32_t imageRowStride,
                                         int32_t fbX, int32_t fbY,
                                         uint32_t fbRowStride,
                                         int32_t width, int32_t height)
{
    const uint8_t* pSrcBase = (const uint8_t*)pSrcImage +
                              imageY * imageRowStride + imageX * 3;
    uint8_t* pDstBase = (uint8_t*)pDstImage +
                        fbY * fbRowStride + fbX * 4;


    for (int32_t h = 0; h < height; ++h)
    {
        const uint8_t* pSrcRow = pSrcBase + h * imageRowStride;
        uint8_t* pDstRow = pDstBase + h * fbRowStride;

        for (int32_t w = 0; w < width; ++w)
        {
            const uint8_t* pSrcPixel = pSrcRow + w * 3;
            uint8_t* pDstPixel = pDstRow + w * 4;

            pDstPixel[0] = pSrcPixel[2];
            pDstPixel[1] = pSrcPixel[1];
            pDstPixel[2] = pSrcPixel[0];
        }
    }
}
static void CpuGrGraphicsBlitRGBA8ToBGR8R(const void* pSrcImage,
                                          void* pDstImage,
                                          int32_t imageX, int32_t imageY,
                                          uint32_t imageRowStride,
                                          int32_t fbX, int32_t fbY,
                                          uint32_t fbRowStride,
                                          int32_t width, int32_t height)
{
    const uint8_t* pSrcBase = (const uint8_t*)pSrcImage +
                              imageY * imageRowStride + imageX * 4;
    uint8_t* pDstBase = (uint8_t*)pDstImage +
                        fbY * fbRowStride + fbX * 4;


    for (int32_t h = 0; h < height; ++h)
    {
        const uint8_t* pSrcRow = pSrcBase + h * imageRowStride;
        uint8_t* pDstRow = pDstBase + h * fbRowStride;

        for (int32_t w = 0; w < width; ++w)
        {
            const uint8_t* pSrcPixel = pSrcRow + w * 4;
            uint8_t* pDstPixel = pDstRow + w * 4;


            const double red = (double)pSrcPixel[0];
            const double green = (double)pSrcPixel[1];
            const double blue = (double)pSrcPixel[2];
            const double alpha = (double)pSrcPixel[3] / 255.0;

            const double alpha_m1 = 1.0 - alpha;

            pDstPixel[0] = (uint8_t)(blue * alpha + (double)pDstPixel[0] * alpha_m1);
            pDstPixel[1] = (uint8_t)(green * alpha + (double)pDstPixel[1] * alpha_m1);
            pDstPixel[2] = (uint8_t)(red * alpha + (double)pDstPixel[2] * alpha_m1);
        }
    }
}


static void CpuGrGraphicsBlitImageToFramebuffer(void* pDrvData,
                                                const void* pImage,
                                                DexprOS_GrImageFormat imageFormat,
                                                int32_t imageX, int32_t imageY,
                                                uint32_t imageRowStride,
                                                int32_t fbX, int32_t fbY,
                                                int32_t width, int32_t height)
{
    DexprOS_CpuGrGraphicsDrvData* pDrv = (DexprOS_CpuGrGraphicsDrvData*)pDrvData;

    int32_t blitImageX = imageX;
    int32_t blitImageY = imageY;
    int32_t blitFbX = fbX;
    int32_t blitFbY = fbY;
    int32_t blitWidth = width;
    int32_t blitHeight = height;

    if (fbX < 0)
    {
        blitImageX -= fbX;
        blitWidth += fbX;
        blitFbX = 0;
    }
    if (fbY < 0)
    {
        blitImageY -= fbY;
        blitHeight += fbY;
        blitFbY = 0;
    }


    if (blitFbX + blitWidth > (int32_t)pDrv->presentationWidth)
        blitWidth = (int32_t)pDrv->presentationWidth - blitFbX;
    if (blitFbY + blitHeight > (int32_t)pDrv->presentationHeight)
        blitHeight = (int32_t)pDrv->presentationHeight - blitFbY;

    // Early returns to mitigate a memory error trying to access data before
    // or after the actual image storage
    if (blitImageX < 0 || blitImageY < 0 || blitWidth < 0 || blitHeight < 0)
        return;
    if (blitFbX >= (int32_t)pDrv->presentationWidth ||
        blitFbY >= (int32_t)pDrv->presentationHeight)
        return;


    CpuGrMarkScreenRegionForSwapping(pDrv,
                                       blitFbX, blitFbY,
                                       blitWidth, blitHeight);


    switch (pDrv->framebufferFormat)
    {
    case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_RGB8_RESERVED8:
        
        switch (imageFormat)
        {
        case DEXPROS_GR_IMAGE_FORMAT_RGB8:
            CpuGrGraphicsBlitRGB8ToRGB8R(pImage,
                                           pDrv->pRenderFramebufferMemory,
                                           blitImageX, blitImageY,
                                           imageRowStride,
                                           blitFbX, blitFbY,
                                           pDrv->presentationPixelStride * 4,
                                           blitWidth, blitHeight);
            return;
        case DEXPROS_GR_IMAGE_FORMAT_RGBA8:
            CpuGrGraphicsBlitRGBA8ToRGB8R(pImage,
                                            pDrv->pRenderFramebufferMemory,
                                            blitImageX, blitImageY,
                                            imageRowStride,
                                            blitFbX, blitFbY,
                                            pDrv->presentationPixelStride * 4,
                                            blitWidth, blitHeight);
            return;
        case DEXPROS_GR_IMAGE_FORMAT_Max:
            return;
        }
        return;

    case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_BGR8_RESERVED8:
        switch (imageFormat)
        {
        case DEXPROS_GR_IMAGE_FORMAT_RGB8:
            CpuGrGraphicsBlitRGB8ToBGR8R(pImage,
                                           pDrv->pRenderFramebufferMemory,
                                           blitImageX, blitImageY,
                                           imageRowStride,
                                           blitFbX, blitFbY,
                                           pDrv->presentationPixelStride * 4,
                                           blitWidth, blitHeight);
            return;
        case DEXPROS_GR_IMAGE_FORMAT_RGBA8:
            CpuGrGraphicsBlitRGBA8ToBGR8R(pImage,
                                           pDrv->pRenderFramebufferMemory,
                                           blitImageX, blitImageY,
                                           imageRowStride,
                                           blitFbX, blitFbY,
                                           pDrv->presentationPixelStride * 4,
                                           blitWidth, blitHeight);
            return;
        case DEXPROS_GR_IMAGE_FORMAT_Max:
            return;
        }
        return;
    
    case DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_Max:
        return;
    }
}



DexprOS_CpuGraphicsDrvInitError DexprOS_InitCpuGraphicsDriver(DexprOS_GraphicsDriver* pDriver,
                                                              EFI_GRAPHICS_OUTPUT_PROTOCOL* pGop,
                                                              EFI_BOOT_SERVICES* pBootServices)
{
    if (pGop->Mode->Info->PixelFormat == PixelBltOnly ||
        pGop->Mode->Info->PixelFormat == PixelBitMask)
        return DEXPROS_CPU_GRAPHICS_DRV_INIT_UNSUPPORTED_FORMAT;


    EFI_STATUS status = 0;
    
    void* pDriverMemory = NULL;
    void* pBackbufferMemory = NULL;
    
    status = pBootServices->AllocatePool(EfiLoaderData, sizeof(DexprOS_CpuGrGraphicsDrvData), &pDriverMemory);
    if (EFI_ERROR(status))
        return DEXPROS_CPU_GRAPHICS_DRV_INIT_MEMORY_ERROR;


    status = pBootServices->AllocatePool(EfiLoaderData, pGop->Mode->FrameBufferSize, &pBackbufferMemory);
    if (EFI_ERROR(status)){
        pBootServices->FreePool(pDriverMemory);
        return DEXPROS_CPU_GRAPHICS_DRV_INIT_MEMORY_ERROR;
    }


    DexprOS_CpuGrGraphicsDrvData* pDriverData = (DexprOS_CpuGrGraphicsDrvData*)pDriverMemory;
    pDriverData->presentationWidth = pGop->Mode->Info->HorizontalResolution;
    pDriverData->presentationHeight = pGop->Mode->Info->VerticalResolution;
    pDriverData->presentationPixelBytes = 4;
    pDriverData->presentationPixelStride = pGop->Mode->Info->PixelsPerScanLine;
    pDriverData->pMainFramebufferMemory = (void*)pGop->Mode->FrameBufferBase;
    pDriverData->mainFramebufferSize = pGop->Mode->FrameBufferSize;
    pDriverData->pRenderFramebufferMemory = pBackbufferMemory;
    pDriverData->swapBufferInfo.swapOp = DEXPROS_CPU_GR_SWAP_BUFFER_NONE_OP;
    pDriverData->swapBufferInfo.regionStartX = 0;
    pDriverData->swapBufferInfo.regionStartY = 0;
    pDriverData->swapBufferInfo.regionWidth = 0;
    pDriverData->swapBufferInfo.regionHeight = 0;
    switch (pGop->Mode->Info->PixelFormat)
    {
        case PixelRedGreenBlueReserved8BitPerColor:
            pDriverData->framebufferFormat = DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_RGB8_RESERVED8;
            break;
        case PixelBlueGreenRedReserved8BitPerColor:
            pDriverData->framebufferFormat = DEXPROS_CPU_GR_FRAMEBUFFER_FORMAT_BGR8_RESERVED8;
            break;
        default:
            break;
    }

    pDriver->swapBuffers = CpuGrSwapBuffers;
    pDriver->clearScreen = CpuGrGraphicsClearScreen;
    pDriver->clearScreenRegions = CpuGrGraphicsClearScreenRegions;
    pDriver->blitImageToFramebuffer = CpuGrGraphicsBlitImageToFramebuffer;
    pDriver->pDriverData = pDriverData;

    return DEXPROS_CPU_GRAPHICS_DRV_INIT_SUCCESS;
}

void DexprOS_DestroyCpuGraphicsDriver(DexprOS_GraphicsDriver* pDriver)
{
    (void)pDriver;
    // Don't release any memory for now as the memory map is owned by
    // the OS when this function is called after ExitBootServices() and
    // FreePool() UEFI function is no longer avaible.
    /*DexprOS_CpuGrGraphicsDrvData* pDriverData = (DexprOS_CpuGrGraphicsDrvData*)pDriver->pDriverData;
    if (pDriverData->pRenderFramebufferMemory != NULL)
        pBootServices->FreePool(pDriverData->pRenderFramebufferMemory);
    
    pBootServices->FreePool(pDriver->pDriverData);*/

    // TODO: implement OS-side memory allocation and free the drivers
    // memory.
}


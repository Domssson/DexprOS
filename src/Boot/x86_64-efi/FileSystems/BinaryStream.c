#include "DexprOS/Boot/x86_64-efi/FileSystems/BinaryStream.h"

#include "DexprOS/Kernel/kstdlib/string.h"


static size_t EmbeddedFileRead(DexprOSBoot_BinaryStream* pStream, void* pData, size_t size, size_t count)
{
    DexprOSBoot_EmbeddedFileBinaryStreamData* pStreamData = (DexprOSBoot_EmbeddedFileBinaryStreamData*)pStream->pStreamData;

    if (pStreamData->pCurrent >= pStreamData->pEnd)
        return 0;

    size_t remainingBytes = pStreamData->pEnd - pStreamData->pCurrent;

    size_t numToRead = remainingBytes / size;
    numToRead = (numToRead < count ? numToRead : count);

    memcpy(pData, pStreamData->pCurrent, numToRead * size);

    pStreamData->pCurrent += numToRead * size;

    return numToRead;
}


static size_t EmbeddedFilePeek(DexprOSBoot_BinaryStream* pStream, void* pData, size_t size, size_t count)
{
    const DexprOSBoot_EmbeddedFileBinaryStreamData* pStreamData = (const DexprOSBoot_EmbeddedFileBinaryStreamData*)pStream->pStreamData;

    if (pStreamData->pCurrent >= pStreamData->pEnd)
        return 0;

    size_t remainingBytes = pStreamData->pEnd - pStreamData->pCurrent;

    size_t numToRead = remainingBytes / size;
    numToRead = (numToRead < count ? numToRead : count);

    memcpy(pData, pStreamData->pCurrent, numToRead * size);

    return numToRead;
}

static void EmbeddedFileSetStreamPos(DexprOSBoot_BinaryStream* pStream, size_t offset)
{
    DexprOSBoot_EmbeddedFileBinaryStreamData* pStreamData = (DexprOSBoot_EmbeddedFileBinaryStreamData*)pStream->pStreamData;
    pStreamData->pCurrent = pStreamData->pStart + offset;
}

static size_t EmbeddedFileGetStreamPos(DexprOSBoot_BinaryStream* pStream)
{
    const DexprOSBoot_EmbeddedFileBinaryStreamData* pStreamData = (const DexprOSBoot_EmbeddedFileBinaryStreamData*)pStream->pStreamData;

    size_t pos = pStreamData->pCurrent - pStreamData->pStart;
    return pos;
} 


DexprOSBoot_BinaryStream DexprOSBoot_CreateEmbeddedFileStream(DexprOSBoot_EmbeddedFileBinaryStreamData* pOutData,
                                                              const char* pStart,
                                                              const char* pEnd)
{
    DexprOSBoot_BinaryStream stream;

    pOutData->pStart = pStart;
    pOutData->pEnd = pEnd;
    pOutData->pCurrent = pStart;

    stream.pStreamData = (void*)pOutData;
    stream.read = EmbeddedFileRead;
    stream.peek = EmbeddedFilePeek;
    stream.setStreamPos = EmbeddedFileSetStreamPos;
    stream.getStreamPos = EmbeddedFileGetStreamPos;

    return stream;
}

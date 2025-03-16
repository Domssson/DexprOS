#ifndef DEXPROSBOOT_X86_64_EFI_FILESYSTEMS_BINARYSTREAM_H_INCLUDED
#define DEXPROSBOOT_X86_64_EFI_FILESYSTEMS_BINARYSTREAM_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


typedef struct DexprOSBoot_BinaryStream
{
    size_t (*read)(struct DexprOSBoot_BinaryStream* pStream, void* pData, size_t size, size_t count);

    size_t (*peek)(struct DexprOSBoot_BinaryStream* pStream, void* pData, size_t size, size_t count);

    void (*setStreamPos)(struct DexprOSBoot_BinaryStream* pStream, size_t offset);

    size_t (*getStreamPos)(struct DexprOSBoot_BinaryStream* pStream);

    void* pStreamData;
} DexprOSBoot_BinaryStream;



typedef struct DexprOSBoot_EmbeddedFileBinaryStreamData
{
    const char* pStart;
    const char* pEnd;

    const char* pCurrent;
} DexprOSBoot_EmbeddedFileBinaryStreamData;

DexprOSBoot_BinaryStream DexprOSBoot_CreateEmbeddedFileStream(DexprOSBoot_EmbeddedFileBinaryStreamData* pOutData,
                                                              const char* pStart,
                                                              const char* pEnd);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEXPROSBOOT_X86_64_EFI_FILESYSTEMS_BINARYSTREAM_H_INCLUDED

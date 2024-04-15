#include "DexprOS/Kernel/kstdlib/string.h"

#include <stdint.h>


void* memset(void* dest, int ch, size_t count)
{
    if (count == 0 || dest == NULL)
        return dest;

    const unsigned char valueToSet = (unsigned char)ch;

    unsigned char* pCurrentDest = (unsigned char*)dest;
    size_t copiedBytes = 0;


    // At the beginning, copy a few bytes so the data pointer is aligned
    // to 8 bytes

    const unsigned bytesToAlignTo8 = ((uintptr_t)pCurrentDest % 8);

    if (bytesToAlignTo8 != 0 || count < bytesToAlignTo8 + 8)
    {
        const uint8_t numBytesToCopy = (count < (bytesToAlignTo8 + 8) ? count : bytesToAlignTo8);
        const unsigned char* const pCopyRegionEnd = pCurrentDest + numBytesToCopy;

        for (; pCurrentDest < pCopyRegionEnd; pCurrentDest += 1)
            *pCurrentDest = valueToSet;
        
        copiedBytes += numBytesToCopy;
    }

    if (copiedBytes == count)
        return dest;


    // Set as much memory as we can through 64-bit integers

    {
        const uint64_t valueToSet64Single = (uint64_t)valueToSet;
        const uint64_t valueToSet64 = valueToSet64Single | (valueToSet64Single << 8) |
                                      (valueToSet64Single << 16) | (valueToSet64Single << 24) |
                                      (valueToSet64Single << 32) | (valueToSet64Single << 40) |
                                      (valueToSet64Single << 48) | (valueToSet64Single << 56);

        const size_t numFitting8Bytes = (count - copiedBytes) / 8;
        uint64_t* pDest8 = (uint64_t*)pCurrentDest;
        const uint64_t* const pCopyRegion8End = pDest8 + numFitting8Bytes;

        for (; pDest8 < pCopyRegion8End; pDest8 += 1)
            *pDest8 = valueToSet64;

        pCurrentDest += numFitting8Bytes * 8;
        copiedBytes += numFitting8Bytes * 8;
    }


    // Copy the rest of unaligned data, if any

    {
        const size_t numBytesLeft = count - copiedBytes;
        const unsigned char* const pCopyRegionEnd = pCurrentDest + numBytesLeft;

        for (; pCurrentDest < pCopyRegionEnd; pCurrentDest += 1)
            *pCurrentDest = valueToSet;
    }

    return dest;
}


#if __STDC_VERSION__ >= 199901L // C99
void* memcpy(void* restrict dest, const void* restrict src, size_t count)
#else
void* memcpy(void* dest, const void* src, size_t count)
#endif
{
    if (dest == NULL || src == NULL || count == 0)
        return dest;

    if (src == dest)
        return dest;
    

    uintptr_t destValue = (uintptr_t)dest;
    uintptr_t sourceValue = (uintptr_t)src;


    // GCC requires that freestanding implementation of memcpy() supports
    // copying data over overlapping memory regions.
    if (sourceValue < destValue + count && sourceValue + count > destValue &&
        destValue > sourceValue)
        return memmove(dest, src, count);
    
    
    if ((destValue % 8) == (sourceValue % 8) && count >= 8)
    {
        uint8_t numAlignBytes = (destValue % 8);

        uint8_t* pAlignDst = (uint8_t*)dest;
        const uint8_t* pAlignSrc = (const uint8_t*)src;
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
        const uint8_t* pAlignSrc = (const uint8_t*)src;
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
        const uint8_t* pAlignSrc = (const uint8_t*)src;
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
        const unsigned char* pSrc = (const unsigned char*)src;
        const unsigned char* pSrcEnd = pSrc + count;

        for (; pSrc != pSrcEnd; ++pSrc, ++pDst)
        {
            *pDst = *pSrc;
        }
    }

    return dest;
}


void* memmove(void* dest, const void* src, size_t count)
{
    if (dest == NULL || src == NULL || count == 0)
        return dest;

    if (src == dest)
        return dest;


    uintptr_t destValue = (uintptr_t)dest;
    uintptr_t sourceValue = (uintptr_t)src;

    
    // Call memcpy if the memory ranges aren't overlapping
    if (sourceValue + count <= destValue || sourceValue >= destValue + count)
        return memcpy(dest, src, count);


    // If the dest memory starts before source memory, it's safe to copy
    // the data forwards from the beggining of the buffer as memcpy() does.
    if (destValue < sourceValue)
        return memcpy(dest, src, count);


    // Copy the data backwards from the end of the buffer

    uintptr_t destEndValue = destValue + count;
    uintptr_t sourceEndValue = sourceValue + count;

    if ((destEndValue % 8) == (sourceEndValue % 8) && count >= 8)
    {
        uint8_t numAlignBytes = (destEndValue % 8);

        uint8_t* pAlignDst = (uint8_t*)dest + count - 1;
        const uint8_t* pAlignSrc = (const uint8_t*)src;
        const uint8_t* pAlignSrcLast = pAlignSrc + numAlignBytes - 1;
        for (; pAlignSrcLast >= pAlignSrc; --pAlignSrcLast, --pAlignDst)
        {
            *pAlignDst = *pAlignSrcLast;
        }

        uint64_t* pDst = (uint64_t*)pAlignDst;
        const uint64_t* pSrc = (const uint64_t*)pAlignSrc;
        const uint64_t* pSrcLast = pSrc + (count - numAlignBytes) / 8 - 1;
        for (; pSrcLast >= pSrc; --pSrcLast, --pDst)
        {
            *pDst = *pSrcLast;
        }

        size_t numRemBytes = ((count - numAlignBytes) % 8);
        uint8_t* pRemDst = (uint8_t*)pDst;
        const uint8_t* pRemSrc = (const uint8_t*)pSrc;
        const uint8_t* pRemSrcLast = pRemSrc + numRemBytes - 1;
        for (; pRemSrcLast >= pRemSrc; --pRemSrcLast, --pRemDst)
        {
            *pRemDst = *pRemSrcLast;
        }
    }
    else if ((destEndValue % 4) == (sourceEndValue % 4) && count >= 4)
    {
        uint8_t numAlignBytes = (destEndValue % 4);

        uint8_t* pAlignDst = (uint8_t*)dest + count - 1;
        const uint8_t* pAlignSrc = (const uint8_t*)src;
        const uint8_t* pAlignSrcLast = pAlignSrc + numAlignBytes - 1;
        for (; pAlignSrcLast >= pAlignSrc; --pAlignSrcLast, --pAlignDst)
        {
            *pAlignDst = *pAlignSrcLast;
        }

        uint32_t* pDst = (uint32_t*)pAlignDst;
        const uint32_t* pSrc = (const uint32_t*)pAlignSrc;
        const uint32_t* pSrcLast = pSrc + (count - numAlignBytes) / 4 - 1;
        for (; pSrcLast >= pSrc; --pSrcLast, --pDst)
        {
            *pDst = *pSrcLast;
        }

        size_t numRemBytes = ((count - numAlignBytes) % 4);
        uint8_t* pRemDst = (uint8_t*)pDst;
        const uint8_t* pRemSrc = (const uint8_t*)pSrc;
        const uint8_t* pRemSrcLast = pRemSrc + numRemBytes - 1;
        for (; pRemSrcLast >= pRemSrc; --pRemSrcLast, --pRemDst)
        {
            *pRemDst = *pRemSrcLast;
        }
    }
    else if ((destEndValue % 2) == (sourceEndValue % 2) && count >= 2)
    {
        uint8_t numAlignBytes = (destEndValue % 2);

        uint8_t* pAlignDst = (uint8_t*)dest + count - 1;
        const uint8_t* pAlignSrc = (const uint8_t*)src;
        const uint8_t* pAlignSrcLast = pAlignSrc + numAlignBytes - 1;
        for (; pAlignSrcLast >= pAlignSrc; --pAlignSrcLast, --pAlignDst)
        {
            *pAlignDst = *pAlignSrcLast;
        }

        uint16_t* pDst = (uint16_t*)pAlignDst;
        const uint16_t* pSrc = (const uint16_t*)pAlignSrc;
        const uint16_t* pSrcLast = pSrc + (count - numAlignBytes) / 2 - 1;
        for (; pSrcLast >= pSrc; --pSrcLast, --pDst)
        {
            *pDst = *pSrcLast;
        }

        size_t numRemBytes = ((count - numAlignBytes) % 2);
        uint8_t* pRemDst = (uint8_t*)pDst;
        const uint8_t* pRemSrc = (const uint8_t*)pSrc;
        const uint8_t* pRemSrcLast = pRemSrc + numRemBytes - 1;
        for (; pRemSrcLast >= pRemSrc; --pRemSrcLast, --pRemDst)
        {
            *pRemDst = *pRemSrcLast;
        }
    }
    else
    {
        unsigned char* pDst = (unsigned char*)dest + count - 1;
        const unsigned char* pSrc = (const unsigned char*)src;
        const unsigned char* pSrcLast = pSrc + count - 1;

        for (; pSrcLast >= pSrc; --pSrcLast, --pDst)
        {
            *pDst = *pSrcLast;
        }
    }


    return dest;
}


int memcmp(const void* lhs, const void* rhs, size_t count)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    if (count == 0 || lhs == rhs)
        return 0;


    uintptr_t lhsValue = (uintptr_t)lhs;
    uintptr_t rhsValue = (uintptr_t)rhs;



    if ((lhsValue % 8) == (rhsValue % 8) && count >= 8)
    {
        uint8_t numAlignBytes = (lhsValue % 8);

        const uint8_t* pAlignLhs = (const uint8_t*)lhs;
        const uint8_t* pAlignRhs = (const uint8_t*)rhs;
        const uint8_t* pAlignLhsEnd = pAlignLhs + numAlignBytes;
        for (; pAlignLhs != pAlignLhsEnd; ++pAlignLhs, ++pAlignRhs)
        {
            if (*pAlignLhs != *pAlignRhs)
                return ((int)*pAlignLhs) - ((int)*pAlignRhs);
        }

        const uint64_t* pLhs = (const uint64_t*)pAlignLhs;
        const uint64_t* pRhs = (const uint64_t*)pAlignRhs;
        const uint64_t* pLhsEnd = pLhs + (count - numAlignBytes) / 8;
        for (; pLhs != pLhsEnd; ++pLhs, ++pRhs)
        {
            if (*pLhs != *pRhs)
            {
                const unsigned char* pU64BytesLhs = (const unsigned char*)pLhs;
                const unsigned char* pU64BytesRhs = (const unsigned char*)pRhs;
                for (unsigned i = 0; i < sizeof(uint64_t); ++i)
                {
                    if (pU64BytesLhs[i] != pU64BytesRhs[i])
                        return ((int)pU64BytesLhs[i]) - ((int)pU64BytesRhs[i]);
                }
            }
        }

        size_t numRemBytes = ((count - numAlignBytes) % 8);
        const uint8_t* pRemLhs = (const uint8_t*)pLhs;
        const uint8_t* pRemRhs = (const uint8_t*)pRhs;
        const uint8_t* pRemLhsEnd = pRemLhs + numRemBytes;
        for (; pRemLhs != pRemLhsEnd; ++pRemLhs, ++pRemRhs)
        {
            if (*pRemLhs != *pRemRhs)
                return ((int)*pRemLhs) - ((int)*pRemRhs);
        }
    }
    else if ((lhsValue % 4) == (rhsValue % 4) && count >= 4)
    {
        uint8_t numAlignBytes = (lhsValue % 4);

        const uint8_t* pAlignLhs = (const uint8_t*)lhs;
        const uint8_t* pAlignRhs = (const uint8_t*)rhs;
        const uint8_t* pAlignLhsEnd = pAlignLhs + numAlignBytes;
        for (; pAlignLhs != pAlignLhsEnd; ++pAlignLhs, ++pAlignRhs)
        {
            if (*pAlignLhs != *pAlignRhs)
                return ((int)*pAlignLhs) - ((int)*pAlignRhs);
        }

        const uint32_t* pLhs = (const uint32_t*)pAlignLhs;
        const uint32_t* pRhs = (const uint32_t*)pAlignRhs;
        const uint32_t* pLhsEnd = pLhs + (count - numAlignBytes) / 4;
        for (; pLhs != pLhsEnd; ++pLhs, ++pRhs)
        {
            if (*pLhs != *pRhs)
            {
                const unsigned char* pU64BytesLhs = (const unsigned char*)pLhs;
                const unsigned char* pU64BytesRhs = (const unsigned char*)pRhs;
                for (unsigned i = 0; i < sizeof(uint32_t); ++i)
                {
                    if (pU64BytesLhs[i] != pU64BytesRhs[i])
                        return ((int)pU64BytesLhs[i]) - ((int)pU64BytesRhs[i]);
                }
            }
        }

        size_t numRemBytes = ((count - numAlignBytes) % 4);
        const uint8_t* pRemLhs = (const uint8_t*)pLhs;
        const uint8_t* pRemRhs = (const uint8_t*)pRhs;
        const uint8_t* pRemLhsEnd = pRemLhs + numRemBytes;
        for (; pRemLhs != pRemLhsEnd; ++pRemLhs, ++pRemRhs)
        {
            if (*pRemLhs != *pRemRhs)
                return ((int)*pRemLhs) - ((int)*pRemRhs);
        }
    }
    else if ((lhsValue % 2) == (rhsValue % 2) && count >= 2)
    {
        uint8_t numAlignBytes = (lhsValue % 2);

        const uint8_t* pAlignLhs = (const uint8_t*)lhs;
        const uint8_t* pAlignRhs = (const uint8_t*)rhs;
        const uint8_t* pAlignLhsEnd = pAlignLhs + numAlignBytes;
        for (; pAlignLhs != pAlignLhsEnd; ++pAlignLhs, ++pAlignRhs)
        {
            if (*pAlignLhs != *pAlignRhs)
                return ((int)*pAlignLhs) - ((int)*pAlignRhs);
        }

        const uint16_t* pLhs = (const uint16_t*)pAlignLhs;
        const uint16_t* pRhs = (const uint16_t*)pAlignRhs;
        const uint16_t* pLhsEnd = pLhs + (count - numAlignBytes) / 2;
        for (; pLhs != pLhsEnd; ++pLhs, ++pRhs)
        {
            if (*pLhs != *pRhs)
            {
                const unsigned char* pU64BytesLhs = (const unsigned char*)pLhs;
                const unsigned char* pU64BytesRhs = (const unsigned char*)pRhs;
                for (unsigned i = 0; i < sizeof(uint16_t); ++i)
                {
                    if (pU64BytesLhs[i] != pU64BytesRhs[i])
                        return ((int)pU64BytesLhs[i]) - ((int)pU64BytesRhs[i]);
                }
            }
        }

        size_t numRemBytes = ((count - numAlignBytes) % 2);
        const uint8_t* pRemLhs = (const uint8_t*)pLhs;
        const uint8_t* pRemRhs = (const uint8_t*)pRhs;
        const uint8_t* pRemLhsEnd = pRemLhs + numRemBytes;
        for (; pRemLhs != pRemLhsEnd; ++pRemLhs, ++pRemRhs)
        {
            if (*pRemLhs != *pRemRhs)
                return ((int)*pRemLhs) - ((int)*pRemRhs);
        }
    }
    else
    {
        const unsigned char* pLHS = (const unsigned char*)lhs;
        const unsigned char* pRHS = (const unsigned char*)rhs;
        const unsigned char* const pLHSEnd = pLHS + count;

        for (; pLHS < pLHSEnd; pLHS += 1, pRHS += 1)
        {
            if (*pLHS != *pRHS)
                return ((int)*pLHS) - ((int)*pRHS);
        }
    }

    return 0;
}



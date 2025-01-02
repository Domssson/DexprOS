#ifndef DEXPROS_KERNEL_X86_64_PAGINGSETTINGS_H_INCLUDED
#define DEXPROS_KERNEL_X86_64_PAGINGSETTINGS_H_INCLUDED

#include <stdbool.h>


typedef enum DexprOS_PagingMode
{
    DEXPROS_PAGING_MODE_4_LEVEL,
    DEXPROS_PAGING_MODE_5_LEVEL
} DexprOS_PagingMode;


typedef struct DexprOS_PagingSettings
{

    DexprOS_PagingMode pagingMode;

    bool noExecuteAvailable;

} DexprOS_PagingSettings;



extern DexprOS_PagingSettings g_DexprOS_PagingSettings;


#endif // DEXPROS_KERNEL_X86_64_PAGINGSETTINGS_H_INCLUDED

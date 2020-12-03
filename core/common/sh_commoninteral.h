#include "sh_link.h"
#include "sh_data.h"
#include "sh_version.h"

#ifdef __cplusplus
extern "C" { 
#endif

void    sh_MemFree(void* pAddress);

void*   sh_MemAllocate(SH_U32 u32Size);

SH_BOOL sh_MemoryPoolInfo(void** pPoolAddr,
                              SH_U32*  pu32PoolSize,
                              SH_U32*  pu32FreeSize,
                              SH_U32*  pu32LargestFreeBlockSize);

SH_S16  sh_DataCreateMutex(void);

void    sh_DataDeleteMutex(SH_S16 s16MutexId);

void    sh_DataLock(SH_S16 s16MutexId);

void    sh_DataUnlock(SH_S16 s16MutexId);

void    sh_DataSetMemFun(SHMemFree pMemFree,SHMemAllocate pMemAlloc,SHMemoryPoolInfo pMemPoolInfo);

SH_BOOL sh_LinkInit(STLink* pstLink);

SH_BOOL sh_LinkDeInit(STLink* pstLink);

SH_BOOL sh_LinkAdd(STLink* pstLink,void* pData,SH_U32 u32Size);

SH_U32  sh_LinkGetNum(STLink* pstLink);


void    sh_OsInit(void);

void    sh_OsDeInit(void);

SH_U32  sh_Malloc(SH_U32 u32Size);

void    sh_Free(void* pAddr);


SH_BOOL sh_FsInit(void);

SH_BOOL sh_FsDeInit(void);


SH_BOOL sh_SocketInit(void);

SH_BOOL sh_SocketDeInit(void);

SH_S8   sh_DataCompare(void* pData1,void* pData2,EDATATYPE eType);

void    sh_DataGetMax(void* pData,EDATATYPE eType);

void    sh_DataGetMin(void* pData,EDATATYPE eType);

SH_BOOL sh_SortCompare(void* pData1,void* pData2,EDATATYPE eType,SH_BOOL bBig);

SH_BOOL sh_SetVersion(SH_U32 u32Pre,SH_U32 u32Last,SH_S8* ps8Module);

#ifdef __cplusplus
} 
#endif 


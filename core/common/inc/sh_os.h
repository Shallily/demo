//2015/10/22 V0.1
//2015/10/24 V0.2 改成动态库版本
//2015/10/27 V0.3 修改Event 的实现
//2015/10/31 V0.4 修改字符串类型为SH_S8
//2015/11/13 V0.5 修改extern "C"  方式
//2016/07/05 V0.6 增加接口SH_GetMallocTimes
//2017/02/28 V0.7
//修改Linux 版本
//2017/03/03 V0.8
//修改Linux 版本创建线程后自启动问题
//2017/07/17 V0.11
//增加获取系统时间接口
//2017/11/29 V0.12
//增加随机数获取接口SH_Rand
//2018/07/06 V0.13
//增加64位版本支持
//2018/07/10 V0.14
//增加结构体STTimeVal
//2018/08/08 V0.15
//增加Windows版本结构体STask成员pParam
#ifndef _SH_OS_H_
#define _SH_OS_H_

#include "sh_common.h"
#if(SH_OS_TYPE == SH_WINDOWS)
#include <windows.h>

#ifdef _SH_OS_DLL_
#define SHOSAPI _declspec(dllexport) 
#else
#define SHOSAPI _declspec(dllimport) 
#pragma comment(lib,"SYS.lib")
#endif

#elif(SH_OS_TYPE == SH_LINUX)
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#define SHOSAPI

#endif

#ifdef __cplusplus
extern "C" { 
#endif

//#define OS_DEBUG

//Thread
#define SH_MAX_THREAD_NUM                       128

#define SH_MAX_THREAD_NAME                      32

#define SH_INVALID_THREAD                       -1

#if(SH_OS_TYPE == SH_WINDOWS)
typedef void ( *SHTaskEntry ) (void* param);
#elif(SH_OS_TYPE == SH_LINUX)
typedef void* ( *SHTaskEntry ) (void* param);
#endif

typedef enum
{
    E_PRI_SYS     = 0,
    E_PRI_HIGHEST = 4,
    E_PRI_HIGH    = 8,
    E_PRI_MEDIUM  = 12,
    E_PRI_LOW     = 16,
    E_PRI_LOWEST  = 24,
} ETaskPriority;

typedef struct
{
    HANDLE  hThread;
    SH_BOOL bUsed;
    SH_S8   as8Name[SH_MAX_THREAD_NAME];
    #if(SH_OS_TYPE == SH_LINUX)
    void*   pStack;
    SH_BOOL bStart;
    SHTaskEntry pTask;
    #elif(SH_OS_TYPE == SH_WINDOWS)
    DWORD   dwThreadId;
    #endif
    void*   pParam;
}STask;

SH_S16  SHOSAPI SH_CreateThread(
    SHTaskEntry pTaskEntry,
    ETaskPriority peTaskPriority,
    VOID* pTaskEntryData,
    SH_S8* ps8Name,
    SH_U32 u32Stacksize,
    SH_BOOL bAutoRun
    );

SH_BOOL SHOSAPI SH_DeleteThread(SH_S16 s16TaskId);

VOID    SHOSAPI SH_ThreadDelay(SH_U32 u32Minisecend);

SH_BOOL SHOSAPI SH_ThreadGetInfo(SH_S16 s16TaskId,STask *stTask);

SH_BOOL SHOSAPI SH_ThreadSuspend(SH_S16 s16TaskId);

SH_BOOL SHOSAPI SH_ThreadResume(SH_S16 s16TaskId);

SH_S16  SHOSAPI SH_ThreadSelf(void);

SH_U16  SHOSAPI SH_GetOpenThreadNum(void);


//Mem
SH_ADDR SHOSAPI SH_Malloc(SH_U32 u32Size);

VOID    SHOSAPI SH_Free(VOID* pAddr);

SH_U32  SHOSAPI SH_GetMallocTimes(void);


//Mutex
#define SH_MAX_MUTEX_NUM                       128

#define SH_MAX_MUTEX_NAME                      32

#define SH_INVALID_MUTEX                       -1

typedef struct
{
    #if(SH_OS_TYPE == SH_LINUX)
    pthread_mutex_t stMutex;
    #elif(SH_OS_TYPE == SH_WINDOWS)
    HANDLE          hMutex;
    #endif
    SH_BOOL         bUsed;
    SH_S8           as8Name[SH_MAX_MUTEX_NAME];
    HANDLE          hOwerThreadHandle;
}STMutex;

SH_S16  SHOSAPI SH_CreateMutex(SH_BOOL bLock,SH_S8* ps8Name);

SH_BOOL SHOSAPI SH_DeleteMutex(SH_S16 s16MutexId);

SH_BOOL SHOSAPI SH_Lock(SH_S16 s16MutexId);

#ifdef OS_DEBUG
SH_BOOL SHOSAPI sh_UnLock(SH_S16 s16MutexId,SH_S8* s8File,SH_U32 u32Line);
#define SH_UnLock(x) sh_UnLock(x,__FILE__,__LINE__)
#else
SH_BOOL SHOSAPI SH_UnLock(SH_S16 s16MutexId);
#endif

SH_BOOL SHOSAPI SH_MutexGetInfo(SH_S16 s16MutexId,STMutex *stMutex);

SH_U16  SHOSAPI SH_GetOpenMutexNum(void);


//Event
#if(SH_OS_TYPE == SH_WINDOWS)

#define SH_MAX_EVENT_NUM                       32

#endif

#define SH_MAX_EVENT_GROUP                     32

#define SH_MAX_EVENT_NAME                      32

#define SH_INVALID_EVENT                       -1

typedef enum
{
    E_MODE_AND,
    E_MODE_OR,
    E_MODE_AND_CLEAR,
    E_MODE_OR_CLEAR
}EWaitMode;

typedef struct
{
    #if(SH_OS_TYPE == SH_WINDOWS)
    HANDLE          hEvent[SH_MAX_EVENT_NUM];
    #elif(SH_OS_TYPE == SH_LINUX)
    pthread_cond_t  stEvent;
    pthread_mutex_t stMutexEvent;
    pthread_mutex_t stMutex;
    SH_U32          u32EventGroup;
    #endif
    SH_BOOL         bUsed;
    SH_S8           as8Name[SH_MAX_EVENT_NAME];
}STEvent;

SH_S16  SHOSAPI SH_CreateEventGroup(SH_U32 u32InitialState,SH_S8* ps8Name);

SH_BOOL SHOSAPI SH_DeleteEventGroup(SH_S16 s16EventId);

SH_BOOL SHOSAPI SH_WaitEvent(SH_S16 s16EventId,EWaitMode eWaitMode,SH_U32 u32WaitEventFlag);

SH_BOOL SHOSAPI SH_SetEvent(SH_S16 s16EventId,SH_U32 u32EventFlag);

SH_BOOL SHOSAPI SH_ClearEvent(SH_S16 s16EventId,SH_U32 u32EventFlag);

SH_BOOL SHOSAPI SH_EventGetInfo(SH_S16 s16EventId,STEvent *stMutex);

SH_U16  SHOSAPI SH_GetOpenEventNum(void);

//Other
typedef struct timeval STTimeVal;
/*typedef struct
{
    SH_U32 u32Sec;
    SH_U32 u32Usec;
}STTimeVal;*/

SH_U32  SHOSAPI SH_GetSysTimeMs(void);

SH_U32  SHOSAPI SH_Rand(void);

#if SH_DEBUG
VOID    SHOSAPI SH_OsSetDebugLevel(SH_U16 u16Level);
#endif

#if(SH_OS_TYPE == SH_LINUX)
VOID    SH_OS_Init(void);
VOID    SH_OS_DeInit(void);
#endif

#ifdef __cplusplus
} 
#endif 

#endif


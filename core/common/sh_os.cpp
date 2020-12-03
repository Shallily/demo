//2015/10/22 V0.1
//2015/10/24 V0.2 改成动态库版本
//2015/10/27 V0.3 修改Event 的实现
//2015/10/31 V0.4 修改字符串类型为SH_S8
//2016/07/05 V0.5 增加接口SH_GetMallocTimes
//2016/07/30 V0.6
//修改创建mutex 返回值为0 时异常问题
//2017/02/28 V0.7
//修改Linux 版本
//2017/02/28 V0.8
//修改Linux mutex返回值错误问题
//2017/03/03 V0.9
//1.修改linux版本创建线程后自启动问题
//2.修改删除线程时线程没执行完导致异常问题
//2017/06/10 V0.10
//解决Windows版本编译问题
//2017/07/17 V0.11
//增加获取系统时间接口
//2017/11/29 V0.12
//增加随机数获取接口SH_Rand
//2018/01/12 V0.13
//修改windows 下随机数获取方法
//2018/01/23 V0.14
//修改linux 下随机数获取方法
//2018/05/27 V0.15
//1.修改Windows 版本获取系统时间错误问题
//2.优化Windows 版本随机数获取算法
//2018/07/06 V0.16
//增加64位版本支持
//2018/07/18 V0.17
//优化Linux线程资源回收问题
//2018/08/08 V0.18
//1.修改接口SH_ThreadGetInfo使之可以获取到线程参数指针
//2.增加Linux调试版本SH_UnLock
//2020/02/19 V0.19
//解决Windows 版本Event 异常问题
#include "config.h"
#if (SH_OS == sh_enable)

#ifndef _SH_OS_DLL_
#define _SH_OS_DLL_
#endif

#include "sh_os.h"
#include "sh_commoninteral.h"
#include "sh_version.h"
#if(SH_OS_TYPE == SH_LINUX)
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <stdlib.h>

#define SH_DEBUG_LEVEL      _u16OsDebugLevel

#if SH_DEBUG
static SH_U16  _u16OsDebugLevel            = SH_DEBUGLEVEL_ERR;
#endif

static SH_BOOL _bOsInit                   = FALSE;

static SH_U32  _u32MemAllocedTimes        = 0;

static STask   _astThreads[SH_MAX_THREAD_NUM];
static STMutex _astMutexes[SH_MAX_MUTEX_NUM];
static STEvent _astEvents[SH_MAX_EVENT_GROUP];

#if(SH_OS_TYPE == SH_WINDOWS)
static HANDLE  _hThreadMutex;
static HANDLE  _hMutexMutex;
static HANDLE  _hEventMutex;
static HANDLE  _hMemMutex;

SH_S16  SH_CreateThread(
    SHTaskEntry  pTaskEntry,
    ETaskPriority peTaskPriority,
    VOID* pTaskEntryData,
    SH_S8* ps8Name,
    SH_U32 u32Stacksize,
    SH_BOOL bAutoRun
    )
{
    SH_U32 u32Index;
    DWORD  dwCreationFlags;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return SH_INVALID_THREAD;
    }
    if (bAutoRun)
    {
        dwCreationFlags = 0;
    }
    else
    {
        dwCreationFlags = CREATE_SUSPENDED;
    }
    WaitForSingleObject(_hThreadMutex,INFINITE);
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        if (!_astThreads[u32Index].bUsed)
        {
            _astThreads[u32Index].hThread = CreateThread(NULL,u32Stacksize,
                (LPTHREAD_START_ROUTINE)pTaskEntry,pTaskEntryData,dwCreationFlags,&(_astThreads[u32Index].dwThreadId));
            if (_astThreads[u32Index].hThread != INVALID_HANDLE_VALUE)
            {
                _astThreads[u32Index].bUsed  = TRUE;
                _astThreads[u32Index].pParam = pTaskEntryData;
                strcpy(_astThreads[u32Index].as8Name,(CHAR*)ps8Name);
                ReleaseMutex(_hThreadMutex);
                SH_DEBUG_INFO(("u32Index = %d hThread = %d\n",u32Index,_astThreads[u32Index].hThread));
                return u32Index;
            }
            ReleaseMutex(_hThreadMutex);
            SH_DEBUG_ERR(("CreateThread err!\n"));
            return SH_INVALID_THREAD;
        }
    }
    ReleaseMutex(_hThreadMutex);
    SH_DEBUG_ERR(("above max thread num!\n"));
    return SH_INVALID_THREAD;
}

SH_BOOL SH_DeleteThread(SH_S16 s16TaskId)
{
    SH_BOOL bRet;
    
    SH_DEBUG_FUNC();

    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d(%s) not be used\n",s16TaskId,_astThreads[s16TaskId].as8Name));
        return TRUE;
    }
    WaitForSingleObject(_hThreadMutex,INFINITE);
    bRet = CloseHandle(_astThreads[s16TaskId].hThread);
    if (bRet)
    {
        _astThreads[s16TaskId].bUsed   = FALSE;
        _astThreads[s16TaskId].hThread = INVALID_HANDLE_VALUE;
    }
    else
    {
        SH_DEBUG_ERR(("CloseHandle err! %s %d\n",__FILE__,__LINE__));
    }
    ReleaseMutex(_hThreadMutex);
    return bRet;
}

VOID    SH_ThreadDelay(SH_U32 u32Minisecend)
{
    //SH_DEBUG_FUNC();
    Sleep(u32Minisecend);
}

SH_BOOL SH_ThreadGetInfo(SH_S16 s16TaskId,STask *stTask)
{
    SH_DEBUG_FUNC();
    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d(%s) not be used\n",s16TaskId,_astThreads[s16TaskId].as8Name));
        return FALSE;
    }
    strcpy(stTask->as8Name,_astThreads[s16TaskId].as8Name);
    stTask->hThread = _astThreads[s16TaskId].hThread;
    stTask->pParam  = _astThreads[s16TaskId].pParam;
    SH_DEBUG_INFO(("as8Name:%s hThread:%d\n",stTask->as8Name,stTask->hThread));
    return TRUE;
}

SH_BOOL SH_ThreadSuspend(SH_S16 s16TaskId)
{
    SH_DEBUG_FUNC();
    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d(%s) not be used\n",s16TaskId,_astThreads[s16TaskId].as8Name));
        return FALSE;
    }
    SuspendThread(_astThreads[s16TaskId].hThread);
    return TRUE;
}

SH_BOOL SH_ThreadResume(SH_S16 s16TaskId)
{
    SH_DEBUG_FUNC();
    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d(%s) not be used\n",s16TaskId,_astThreads[s16TaskId].as8Name));
        return FALSE;
    }
    ResumeThread(_astThreads[s16TaskId].hThread);
    return TRUE;
}

SH_S16  SH_ThreadSelf(VOID)
{
    SH_U32 u32Index;
    DWORD  dwCurThreadId;
    
    SH_DEBUG_FUNC();
    dwCurThreadId = GetCurrentThreadId();
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        if (_astThreads[u32Index].bUsed && (_astThreads[u32Index].dwThreadId == dwCurThreadId))
        {
            SH_DEBUG_INFO(("u32Index = %d\n",u32Index));
            return u32Index;
        }
    }
    SH_DEBUG_ERR(("bug!!!\n"));
    return SH_INVALID_THREAD;
}

SH_U16  SH_GetOpenThreadNum(void)
{
    SH_U32 u32Index;
    SH_U32 u32ThreadNum = 0;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return 0;
    }
    WaitForSingleObject(_hThreadMutex,INFINITE);
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        if (_astThreads[u32Index].bUsed)
        {
            u32ThreadNum++;
        }
    }
    ReleaseMutex(_hThreadMutex);
    SH_DEBUG_INFO(("u32ThreadNum = %d\n",u32ThreadNum));
    return u32ThreadNum;
}


SH_U32  sh_Malloc(SH_U32 u32Size)
{
    VOID* pRet;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return NULL;
    }
    WaitForSingleObject(_hMemMutex,INFINITE);
    pRet = HeapAlloc(GetProcessHeap(),0,u32Size);
    if (pRet != NULL)
    {
        _u32MemAllocedTimes++;
    }
    else
    {
        SH_DEBUG_ERR(("HeapAlloc err!\n"));
    }
    ReleaseMutex(_hMemMutex);
    return (SH_U32)pRet;
}

SH_U32  SH_Malloc(SH_U32 u32Size)
{
    return sh_Malloc(u32Size);
}

VOID    sh_Free(VOID* pAddr)
{
    SH_DEBUG_FUNC();
    if (pAddr == NULL)
    {
        SH_DEBUG_WRN(("pAddr is NULL\n"));
        return;
    }
    WaitForSingleObject(_hMemMutex,INFINITE);
    if (HeapFree(GetProcessHeap(),0,pAddr))
    {
        _u32MemAllocedTimes--;
    }
    else
    {
        SH_DEBUG_ERR(("HeapFree err!\n"));
    }
    ReleaseMutex(_hMemMutex);
}

VOID    SH_Free(VOID* pAddr)
{
    sh_Free(pAddr);
}

SH_S16  SH_CreateMutex(SH_BOOL bLock,SH_S8* ps8Name)
{
    SH_U32 u32Index;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return SH_INVALID_MUTEX;
    }
    WaitForSingleObject(_hMutexMutex,INFINITE);
    for (u32Index = 0;u32Index < SH_MAX_MUTEX_NUM;u32Index++)
    {
        if (!_astMutexes[u32Index].bUsed)
        {
            _astMutexes[u32Index].hMutex = CreateMutex(NULL,bLock,ps8Name);
            if ((_astMutexes[u32Index].hMutex != INVALID_HANDLE_VALUE) &&
                (_astMutexes[u32Index].hMutex != 0))
            {
                _astMutexes[u32Index].bUsed = TRUE;
                strcpy(_astMutexes[u32Index].as8Name,ps8Name);
                ReleaseMutex(_hMutexMutex);
                SH_DEBUG_INFO(("u32Index = %d\n",u32Index));
                return u32Index;
            }
            ReleaseMutex(_hMutexMutex);
            SH_DEBUG_ERR(("CreateMutex err!\n"));
            return SH_INVALID_MUTEX;
        }
    }
    ReleaseMutex(_hMutexMutex);
    SH_DEBUG_ERR(("above max mutex num!\n"));
    return SH_INVALID_MUTEX;
}

SH_BOOL SH_DeleteMutex(SH_S16 s16MutexId)
{
    SH_BOOL bRet;
    
    SH_DEBUG_FUNC();

    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_WRN(("mutex %d(%s) not be used\n",s16MutexId,_astMutexes[s16MutexId].as8Name));
        return TRUE;
    }
    WaitForSingleObject(_hMutexMutex,INFINITE);
    bRet = CloseHandle(_astMutexes[s16MutexId].hMutex);
    if (bRet)
    {
        _astMutexes[s16MutexId].bUsed  = FALSE;
        _astMutexes[s16MutexId].hMutex = INVALID_HANDLE_VALUE;
    }
    else
    {
        SH_DEBUG_ERR(("CloseHandle err! %s %d\n",__FILE__,__LINE__));
    }
    ReleaseMutex(_hMutexMutex);
    return bRet;
}

SH_BOOL SH_Lock(SH_S16 s16MutexId)
{
    DWORD dwRet;
    
    //SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_ERR(("mutex %d(%s) not be used\n",s16MutexId,_astMutexes[s16MutexId].as8Name));
        return FALSE;
    }
    if (_astMutexes[s16MutexId].hOwerThreadHandle == (HANDLE)GetCurrentThreadId())
    {
        SH_DEBUG_WRN(("thread %d has already obtained mutex %d\n",GetCurrentThreadId(),s16MutexId));
        SH_DEBUG_WRN(("thread name:%s\n",_astThreads[SH_ThreadSelf()].as8Name));
        return TRUE;
    }
    dwRet = WaitForSingleObject(_astMutexes[s16MutexId].hMutex,INFINITE);
    if (dwRet == WAIT_OBJECT_0)
    {
        _astMutexes[s16MutexId].hOwerThreadHandle = (HANDLE)GetCurrentThreadId();
        return TRUE;
    }
    SH_DEBUG_ERR(("system err!!! %s %d\n",__FILE__,__LINE__));
    return FALSE;
}

#ifdef OS_DEBUG
SH_BOOL sh_UnLock(SH_S16 s16MutexId,SH_S8* s8File,SH_U32 u32Line)
{
    SH_BOOL bRet;
    
    //SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_ERR(("mutex %d(%s) not be used\n",s16MutexId,_astMutexes[s16MutexId].as8Name));
        return FALSE;
    }
    if (_astMutexes[s16MutexId].hOwerThreadHandle != (HANDLE)GetCurrentThreadId())
    {
        SH_DEBUG_ERR(("thread %d has not obtained mutex %d\n%s %d\n",GetCurrentThreadId(),s16MutexId,s8File,u32Line));
        SH_DEBUG_ERR(("thread name:%s\n",_astThreads[SH_ThreadSelf()].as8Name));
        return FALSE;
    }
    _astMutexes[s16MutexId].hOwerThreadHandle = INVALID_HANDLE_VALUE;
    bRet = ReleaseMutex(_astMutexes[s16MutexId].hMutex);
    if (!bRet)
    {
        SH_DEBUG_ERR(("system err!!! %s %d\n",__FILE__,__LINE__));
    }
    return bRet;
}
#else
SH_BOOL SH_UnLock(SH_S16 s16MutexId)
{
    SH_BOOL bRet;
    
    //SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_ERR(("mutex %d(%s) not be used\n",s16MutexId,_astMutexes[s16MutexId].as8Name));
        return FALSE;
    }
    if (_astMutexes[s16MutexId].hOwerThreadHandle != (HANDLE)GetCurrentThreadId())
    {
        SH_DEBUG_ERR(("thread %d has not obtained mutex %d\n%s %d\n",GetCurrentThreadId(),s16MutexId,__FILE__,__LINE__));
        SH_DEBUG_ERR(("thread name:%s\n",_astThreads[SH_ThreadSelf()].as8Name));
        return FALSE;
    }
    _astMutexes[s16MutexId].hOwerThreadHandle = INVALID_HANDLE_VALUE;
    bRet = ReleaseMutex(_astMutexes[s16MutexId].hMutex);
    if (!bRet)
    {
        SH_DEBUG_ERR(("system err!!! %s %d\n",__FILE__,__LINE__));
    }
    return bRet;
}
#endif

SH_BOOL SH_MutexGetInfo(SH_S16 s16MutexId,STMutex *stMutex)
{
    SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_ERR(("mutex %d(%s) not be used\n",s16MutexId,_astMutexes[s16MutexId].as8Name));
        return FALSE;
    }
    strcpy(stMutex->as8Name,_astMutexes[s16MutexId].as8Name);
    stMutex->hMutex            = _astMutexes[s16MutexId].hMutex;
    stMutex->hOwerThreadHandle = _astMutexes[s16MutexId].hOwerThreadHandle;
    SH_DEBUG_INFO(("as8Name:%s hMutex:%d hOwerThreadHandle:%d\n",
        stMutex->as8Name,stMutex->hMutex,stMutex->hOwerThreadHandle));
    return TRUE;
}

SH_U16  SH_GetOpenMutexNum(void)
{
    SH_U32 u32Index;
    SH_U32 u32MutexNum = 0;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_WRN(("os not init\n"));
        return 0;
    }
    WaitForSingleObject(_hMutexMutex,INFINITE);
    for (u32Index = 0;u32Index < SH_MAX_MUTEX_NUM;u32Index++)
    {
        if (_astMutexes[u32Index].bUsed)
        {
            u32MutexNum++;
        }
    }
    ReleaseMutex(_hMutexMutex);
    SH_DEBUG_INFO(("u32MutexNum = %d\n",u32MutexNum));
    return u32MutexNum;
}


SH_S16  SH_CreateEventGroup(SH_U32 u32InitialState,SH_S8* ps8Name)
{
    SH_U32  u32Idx;
    SH_U32  u32Index;
    SH_BOOL bInitialState;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return SH_INVALID_EVENT;
    }
    WaitForSingleObject(_hEventMutex,INFINITE);
    for (u32Index = 0;u32Index < SH_MAX_EVENT_GROUP;u32Index++)
    {
        if (!_astEvents[u32Index].bUsed)
        {
            for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
            {
                bInitialState = u32InitialState & (0x0001 << u32Idx);
                _astEvents[u32Index].hEvent[u32Idx] = CreateEvent(NULL,true,bInitialState,NULL);
                if (_astEvents[u32Index].hEvent[u32Idx] == INVALID_HANDLE_VALUE)
                {
                    while(--u32Idx)
                    {
                        CloseHandle(_astEvents[u32Index].hEvent[u32Idx]);
                        _astEvents[u32Index].hEvent[u32Idx] = INVALID_HANDLE_VALUE;
                    }
                    CloseHandle(_astEvents[u32Index].hEvent[0]);
                    _astEvents[u32Index].hEvent[0] = INVALID_HANDLE_VALUE;
                    ReleaseMutex(_hEventMutex);
                    SH_DEBUG_ERR(("CreateEvent err!\n"));
                    return SH_INVALID_EVENT;
                }
            }
            _astEvents[u32Index].bUsed = TRUE;
            strcpy(_astEvents[u32Index].as8Name,ps8Name);
            ReleaseMutex(_hEventMutex);
            SH_DEBUG_INFO(("u32Index = %d\n",u32Index));
            return u32Index;
        }
    }
    ReleaseMutex(_hEventMutex);
    SH_DEBUG_ERR(("above max event num!\n"));
    return SH_INVALID_EVENT;
}

SH_BOOL SH_DeleteEventGroup(SH_S16 s16EventId)
{
    SH_BOOL bRet   = TRUE;
    SH_U32  u32Idx;
    
    SH_DEBUG_FUNC();

    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d(%s) not be used\n",s16EventId,_astEvents[s16EventId].as8Name));
        return TRUE;
    }
    WaitForSingleObject(_hEventMutex,INFINITE);
    for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
    {
        bRet = (bRet && CloseHandle(_astEvents[s16EventId].hEvent[u32Idx]));
        if (bRet)
        {
            _astEvents[s16EventId].hEvent[u32Idx] = INVALID_HANDLE_VALUE;
        }
        else
        {
            SH_DEBUG_WRN(("CloseHandle err u32Idx = %d\n",u32Idx));
        }
    }
    if (bRet)
    {
        _astEvents[s16EventId].bUsed  = FALSE;
    }
    else
    {
        SH_DEBUG_ERR(("CloseHandle err! %s %d\n",__FILE__,__LINE__));
    }
    ReleaseMutex(_hEventMutex);
    return bRet;
}

SH_BOOL SH_WaitEvent(SH_S16 s16EventId,EWaitMode eWaitMode,SH_U32 u32WaitEventFlag)
{
    DWORD  dwRet;
    SH_U32 u32Idx;
    HANDLE hEvents[SH_MAX_EVENT_NUM];
    DWORD  dwConut                    = 0;
    BOOL   bWatiAll                   = FALSE;

    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d(%s) not be used\n",s16EventId,_astEvents[s16EventId].as8Name));
        return FALSE;
    }
    for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
    {
        if (u32WaitEventFlag & (0x0001 << u32Idx))
        {
            hEvents[dwConut++] = _astEvents[s16EventId].hEvent[u32Idx];
            SH_DEBUG_INFO(("SH_WaitEvent hEvent = %d\n",_astEvents[s16EventId].hEvent[u32Idx]));
        }
    }
    if ((eWaitMode == E_MODE_AND) || (eWaitMode == E_MODE_AND_CLEAR))
    {
        bWatiAll = TRUE;
    }
    dwRet = WaitForMultipleObjects(dwConut,hEvents,bWatiAll,INFINITE);
    if ((eWaitMode == E_MODE_OR_CLEAR) || (eWaitMode == E_MODE_AND_CLEAR))
    {
        for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
        {
            if (u32WaitEventFlag & (0x0001 << u32Idx))
            {
                ResetEvent(_astEvents[s16EventId].hEvent[u32Idx]);
            }
        }
    }
    if (dwRet == WAIT_OBJECT_0)
        return TRUE;
    SH_DEBUG_ERR(("system err!!! %s %d %d %d %d\n",__FILE__,__LINE__,dwRet,WAIT_OBJECT_0,GetLastError()));
    return FALSE;
}

SH_BOOL SH_SetEvent(SH_S16 s16EventId,SH_U32 u32EventFlag)
{
    BOOL   bRet   = TRUE;
    SH_U32 u32Idx;
    
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d(%s) not be used\n",s16EventId,_astEvents[s16EventId].as8Name));
        return FALSE;
    }
    for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
    {
        if (u32EventFlag & (0x0001 << u32Idx))
        {
            SH_DEBUG_INFO(("SH_SetEvent hEvent = %d\n",_astEvents[s16EventId].hEvent[u32Idx]));
            bRet = (bRet && SetEvent(_astEvents[s16EventId].hEvent[u32Idx]));
            if (!bRet)
            {
                SH_DEBUG_WRN(("SetEvent err u32Idx = %d\n",u32Idx));
            }
        }
    }
    if (bRet)
        return TRUE;
    SH_DEBUG_ERR(("system err!!! %s %d\n",__FILE__,__LINE__));
    return FALSE;
}

SH_BOOL SH_ClearEvent(SH_S16 s16EventId,SH_U32 u32EventFlag)
{
    BOOL   bRet   = TRUE;
    SH_U32 u32Idx;
    
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d(%s) not be used\n",s16EventId,_astEvents[s16EventId].as8Name));
        return FALSE;
    }
    for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
    {
        if (!(u32EventFlag & (0x0001 << u32Idx)))
        {
            SH_DEBUG_INFO(("SH_ClearEvent hEvent = %d\n",_astEvents[s16EventId].hEvent[u32Idx]));
            bRet = (bRet && ResetEvent(_astEvents[s16EventId].hEvent[u32Idx]));
            if (!bRet)
            {
                SH_DEBUG_WRN(("ResetEvent err u32Idx = %d\n",u32Idx));
            }
        }
    }
    if (bRet)
        return TRUE;
    SH_DEBUG_ERR(("system err!!! %s %d\n",__FILE__,__LINE__));
    return FALSE;
}


SH_BOOL SH_EventGetInfo(SH_S16 s16EventId,STEvent *stEvent)
{
    SH_U32 u32Idx;
    
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d(%s) not be used\n",s16EventId,_astEvents[s16EventId].as8Name));
        return FALSE;
    }
    strcpy(stEvent->as8Name,_astEvents[s16EventId].as8Name);
    for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
    {
        stEvent->hEvent[u32Idx] = _astEvents[s16EventId].hEvent[u32Idx];
        SH_DEBUG_INFO(("as8Name:%s hEvent[%d]:%d\n",stEvent->as8Name,u32Idx,stEvent->hEvent[u32Idx]));
    }
    return TRUE;
}

SH_U16  SH_GetOpenEventNum(void)
{
    SH_U32 u32Index;
    SH_U32 u32EventNum = 0;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_WRN(("os not init\n"));
        return 0;
    }
    WaitForSingleObject(_hEventMutex,INFINITE);
    for (u32Index = 0;u32Index < SH_MAX_EVENT_GROUP;u32Index++)
    {
        if (_astEvents[u32Index].bUsed)
        {
            u32EventNum++;
        }
    }
    ReleaseMutex(_hEventMutex);
    SH_DEBUG_INFO(("u32EventNum = %d\n",u32EventNum));
    return u32EventNum;
}

SH_U32  SH_GetSysTimeMs(void)
{
    #if 0
    SYSTEMTIME stSysTime;
    
    GetLocalTime(&stSysTime);
    #else
    return clock();
    #endif
}

VOID    sh_OsInit(VOID)
{
    SH_U32 u32Idx;
    SH_U32 u32Index;
    time_t stRawTime;
    struct tm * stTimeInfo;
    
    SH_DEBUG_FUNC();
    if (_bOsInit)
    {
        return;
    }
    
    _bOsInit = TRUE;
    memset(_astThreads,0,sizeof(_astThreads));
    memset(_astMutexes,0,sizeof(_astMutexes));
    memset(_astEvents,0,sizeof(_astEvents));
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        _astThreads[u32Index].hThread           = INVALID_HANDLE_VALUE;
    }
    
    for (u32Index = 0;u32Index < SH_MAX_MUTEX_NUM;u32Index++)
    {
        _astMutexes[u32Index].hMutex            = INVALID_HANDLE_VALUE;
        _astMutexes[u32Index].hOwerThreadHandle = INVALID_HANDLE_VALUE;
    }
    
    for (u32Index = 0;u32Index < SH_MAX_EVENT_GROUP;u32Index++)
    {
        for (u32Idx = 0;u32Idx < SH_MAX_EVENT_NUM;u32Idx++)
        {
            _astEvents[u32Index].hEvent[u32Idx] = INVALID_HANDLE_VALUE;
        }
    }
    
    _hThreadMutex = CreateMutex(NULL,FALSE,"thread mutex");
    _hMutexMutex  = CreateMutex(NULL,FALSE,"mutex mutex");
    _hEventMutex  = CreateMutex(NULL,FALSE,"event mutex");
    _hMemMutex    = CreateMutex(NULL,FALSE,"mem mutex");

    sh_SetVersion(0,19,LSTRING"os");
    time(&stRawTime);
    stTimeInfo = localtime(&stRawTime);
    srand(stTimeInfo->tm_sec * stTimeInfo->tm_min);
}

VOID    sh_OsDeInit(VOID)
{
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        return;
    }
    CloseHandle(_hThreadMutex);
    CloseHandle(_hMutexMutex);
    CloseHandle(_hEventMutex);
    CloseHandle(_hMemMutex);
    _bOsInit = FALSE;
}
#elif(SH_OS_TYPE == SH_LINUX)
typedef enum
{
    E_LINUX_PRI_SYS      = 99,
    E_LINUX_PRI_HIGHEST  = 80,
    E_LINUX_PRI_HIGH     = 60,
    E_LINUX_PRI_MEDIUM   = 40,
    E_LINUX_PRI_LOW      = 20,
    E_LINUX_PRI_LOWEST   = 1,
} _LinuxPriority;

static pthread_mutex_t  _stThreadMutex;
static pthread_mutex_t  _stMutexMutex;
static pthread_mutex_t  _stEventMutex;
static pthread_mutex_t  _stMemMutex;

static void _sh_ThreadDelay(SH_U32 u32Minisecend)
{
    SH_S32 s32Err;
    
    SH_DEBUG_FUNC();
    struct timespec req, rem;
    if(u32Minisecend >= 1000)
    {
        req.tv_sec = u32Minisecend/1000;
        req.tv_nsec = (long) ((u32Minisecend%1000)*1000000UL);
    }
    else
    {
        req.tv_sec = 0;
        req.tv_nsec = (long) (u32Minisecend*1000000UL);
    }

    while(1)
    {
        s32Err = nanosleep(&req, &rem);
        if(s32Err == -1)
        {
            switch(errno)
            {
                case EINTR:
                    req.tv_sec = rem.tv_sec;
                    req.tv_nsec = rem.tv_nsec;
                   continue;
                default:
                    if(errno !=(int)NULL)
                    break;
            }

        }

        break;
    }
}


static void* _sh_Task(void* pPara)
{
    STask* pstTask = (STask*)pPara;

    while (pstTask->bStart == FALSE)
    {
        _sh_ThreadDelay(100);
    }

    pstTask->pTask(pstTask->pParam);
    return NULL;
}

SH_S16  SH_CreateThread(
    SHTaskEntry pTaskEntry,
    ETaskPriority peTaskPriority,
    VOID* pTaskEntryData,
    SH_S8* ps8Name,
    SH_U32 u32Stacksize,
    SH_BOOL bAutoRun
    )
{
    SH_U32             u32Index;
    pthread_attr_t     stThreadAttr;
    struct sched_param stThrsched;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return SH_INVALID_THREAD;
    }
    
    pthread_mutex_lock(&_stThreadMutex);
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        if (!_astThreads[u32Index].bUsed)
        {
            _astThreads[u32Index].pStack  = malloc(u32Stacksize);
            pthread_attr_init(&stThreadAttr);
            pthread_attr_setstack(&stThreadAttr,_astThreads[u32Index].pStack,u32Stacksize);
            pthread_attr_setdetachstate(&stThreadAttr, PTHREAD_CREATE_DETACHED);
            //pthread_attr_setinheritsched(&stThreadAttr, PTHREAD_EXPLICIT_SCHED);
            switch(peTaskPriority)
            {
                case E_PRI_SYS:
                case E_PRI_HIGHEST:
                    pthread_attr_setschedpolicy(&stThreadAttr, SCHED_RR);
                    break;
                default:
                    pthread_attr_setschedpolicy(&stThreadAttr, SCHED_OTHER);
                    break;
            }
            
            pthread_attr_getschedparam(&stThreadAttr, &stThrsched);
            switch(peTaskPriority)
            {
                case E_PRI_SYS:
                    stThrsched.sched_priority = E_LINUX_PRI_SYS;
                    break;
                case E_PRI_HIGHEST:
                    stThrsched.sched_priority = E_LINUX_PRI_HIGHEST;
                    break;
                case E_PRI_HIGH:
                case E_PRI_MEDIUM:
                case E_PRI_LOW:
                case E_PRI_LOWEST:
                default:
                    stThrsched.sched_priority = sched_get_priority_min(SCHED_OTHER);
                    break;
            }
    
            pthread_attr_setschedparam(&stThreadAttr, &stThrsched);
            if (bAutoRun)
            {
                _astThreads[u32Index].bStart = TRUE;
            }
            else
            {
                _astThreads[u32Index].bStart = FALSE;
            }

            _astThreads[u32Index].pParam = pTaskEntryData;
            _astThreads[u32Index].bUsed  = TRUE;
            _astThreads[u32Index].pTask  = pTaskEntry;
            strcpy(_astThreads[u32Index].as8Name,ps8Name);
            //修改linux task 自启动问题
            pthread_create((pthread_t*)(&_astThreads[u32Index].hThread),
                               &stThreadAttr,
                               _sh_Task,
                               (void *)&(_astThreads[u32Index]));
            pthread_attr_destroy(&stThreadAttr);
            pthread_mutex_unlock(&_stThreadMutex);
            SH_DEBUG_INFO(("u32Index = %d\n",(int)u32Index));
            return u32Index;
        }
    }
    
    pthread_mutex_unlock(&_stThreadMutex);
    SH_DEBUG_ERR(("above max thread num!\n"));
    return SH_INVALID_THREAD;
}

SH_BOOL SH_DeleteThread(SH_S16 s16TaskId)
{
    SH_DEBUG_FUNC();

    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d not be used\n",s16TaskId));
        return TRUE;
    }
    
    pthread_mutex_lock(&_stThreadMutex);
    //修改删除线程时线程没执行完导致异常问题
    //pthread_cancel((pthread_t)_astThreads[s16TaskId].hThread);
    _astThreads[s16TaskId].bUsed   = FALSE;
    _astThreads[s16TaskId].hThread = (HANDLE)SH_INVALID_THREAD;
    free(_astThreads[s16TaskId].pStack);
    _astThreads[s16TaskId].pStack  = NULL;    
    pthread_mutex_unlock(&_stThreadMutex);
    return TRUE;
}

VOID    SH_ThreadDelay(SH_U32 u32Minisecend)
{
    _sh_ThreadDelay(u32Minisecend);
    //usleep(u32Minisecend * 1000);
}

SH_BOOL SH_ThreadGetInfo(SH_S16 s16TaskId,STask *stTask)
{
    SH_DEBUG_FUNC();
    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d not be used\n",s16TaskId));
        return FALSE;
    }
    
    strcpy(stTask->as8Name,_astThreads[s16TaskId].as8Name);
    stTask->hThread = _astThreads[s16TaskId].hThread;
    stTask->pParam  = _astThreads[s16TaskId].pParam;
    SH_DEBUG_INFO(("as8Name:%s hThread:%p\n",stTask->as8Name,stTask->hThread));
    return TRUE;
}

SH_BOOL SH_ThreadSuspend(SH_S16 s16TaskId)
{
    SH_DEBUG_FUNC();
    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d not be used\n",s16TaskId));
        return FALSE;
    }

    //TODO
    return TRUE;
}

SH_BOOL SH_ThreadResume(SH_S16 s16TaskId)
{
    SH_DEBUG_FUNC();
    if (!_astThreads[s16TaskId].bUsed)
    {
        SH_DEBUG_WRN(("thread %d not be used\n",s16TaskId));
        return FALSE;
    }

    _astThreads[s16TaskId].bStart = TRUE;
    //TODO
    return TRUE;
}

SH_S16  SH_ThreadSelf(void)
{
    SH_U32 u32Index;
    HANDLE hCurThread;
    
    SH_DEBUG_FUNC();
    hCurThread = (HANDLE)pthread_self();
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        if (_astThreads[u32Index].bUsed && (_astThreads[u32Index].hThread == hCurThread))
        {
            SH_DEBUG_INFO(("u32Index = %d\n",(int)u32Index));
            return u32Index;
        }
    }
    
    SH_DEBUG_ERR(("bug!!!\n"));
    return SH_INVALID_THREAD;
}

SH_U16  SH_GetOpenThreadNum(void)
{
    SH_U32 u32Index;
    SH_U32 u32ThreadNum = 0;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_WRN(("os not init\n"));
        return 0;
    }
    
    pthread_mutex_lock(&_stThreadMutex);
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        if (_astThreads[u32Index].bUsed)
        {
            u32ThreadNum++;
        }
    }
    
    pthread_mutex_unlock(&_stThreadMutex);
    SH_DEBUG_INFO(("u32ThreadNum = %d\n",(int)u32ThreadNum));
    return u32ThreadNum;
}


SH_ADDR  SH_Malloc(SH_U32 u32Size)
{
    VOID* pRet;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return 0;
    }
    
    pthread_mutex_lock(&_stMemMutex);
    pRet = malloc(u32Size);
    if (pRet != NULL)
    {
        _u32MemAllocedTimes++;
    }
    else
    {
        SH_DEBUG_ERR(("malloc err!\n"));
    }
    
    pthread_mutex_unlock(&_stMemMutex);
    return (SH_ADDR)pRet;
}

VOID    SH_Free(VOID* pAddr)
{
    SH_DEBUG_FUNC();
    if (pAddr == NULL)
    {
        SH_DEBUG_WRN(("pAddr is NULL\n"));
        return;
    }
    
    pthread_mutex_lock(&_stMemMutex);
    free(pAddr);
    _u32MemAllocedTimes--;
    pthread_mutex_unlock(&_stMemMutex);
}


SH_S16  SH_CreateMutex(SH_BOOL bLock,SH_S8* ps8Name)
{
    SH_U32              u32Index;
    pthread_mutexattr_t stAttr;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return SH_INVALID_MUTEX;
    }
    pthread_mutex_lock(&_stMutexMutex);
    for (u32Index = 0;u32Index < SH_MAX_MUTEX_NUM;u32Index++)
    {
        if (!_astMutexes[u32Index].bUsed)
        {
            pthread_mutexattr_init(&stAttr);
            pthread_mutex_init(&_astMutexes[u32Index].stMutex, &stAttr);
            _astMutexes[u32Index].bUsed = TRUE;
            strcpy(_astMutexes[u32Index].as8Name,ps8Name);
            pthread_mutex_unlock(&_stMutexMutex);
            SH_DEBUG_INFO(("u32Index = %d\n",(int)u32Index));
            return u32Index;
        }
    }
    
    pthread_mutex_unlock(&_stMutexMutex);
    SH_DEBUG_ERR(("above max mutex num!\n"));
    return SH_INVALID_MUTEX;
}

SH_BOOL SH_DeleteMutex(SH_S16 s16MutexId)
{
    SH_DEBUG_FUNC();

    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_WRN(("mutex %d not be used\n",s16MutexId));
        return TRUE;
    }
    
    pthread_mutex_lock(&_stMutexMutex);
    pthread_mutex_destroy(&_astMutexes[s16MutexId].stMutex);
    _astMutexes[s16MutexId].bUsed  = FALSE;
    pthread_mutex_unlock(&_stMutexMutex);
    return TRUE;
}

SH_BOOL SH_Lock(SH_S16 s16MutexId)
{
    SH_BOOL bRet;
    
    SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_ERR(("mutex %d not be used\n",s16MutexId));
        return FALSE;
    }
    
    if (_astMutexes[s16MutexId].hOwerThreadHandle == (HANDLE)pthread_self())
    {
        SH_DEBUG_WRN(("thread %d has already obtained mutex %d\n",(int)pthread_self(),(int)s16MutexId));
        return TRUE;
    }
    
    bRet = pthread_mutex_lock(&_astMutexes[s16MutexId].stMutex);
    if (!bRet)
    {
        _astMutexes[s16MutexId].hOwerThreadHandle = (HANDLE)pthread_self();
    }
    
    return bRet;
}

#ifdef OS_DEBUG
SH_BOOL sh_UnLock(SH_S16 s16MutexId,SH_S8* s8File,SH_U32 u32Line)
#else
SH_BOOL SH_UnLock(SH_S16 s16MutexId)
#endif
{
    SH_BOOL bRet;
    
    SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        #ifdef OS_DEBUG
        SH_DEBUG_ERR(("mutex %d not be used %s %s %d\n",s16MutexId,_astMutexes[s16MutexId].as8Name,s8File,u32Line));
        #else
        SH_DEBUG_ERR(("mutex %d not be used %s\n",s16MutexId,_astMutexes[s16MutexId].as8Name));
        #endif
        return FALSE;
    }
    
    if (_astMutexes[s16MutexId].hOwerThreadHandle != (HANDLE)pthread_self())
    {
        SH_DEBUG_ERR(("thread %d has not obtained mutex %d %s\n",(int)pthread_self(),(int)s16MutexId,_astMutexes[s16MutexId].as8Name));
        return FALSE;
    }
    
    _astMutexes[s16MutexId].hOwerThreadHandle = (HANDLE)SH_INVALID_THREAD;
    bRet = pthread_mutex_unlock(&_astMutexes[s16MutexId].stMutex);
    if (bRet)
    {
        SH_DEBUG_ERR(("system err!!! %s %d\n",__FILE__,__LINE__));
    }
    
    return bRet;
}

SH_BOOL SH_MutexGetInfo(SH_S16 s16MutexId,STMutex *stMutex)
{
    SH_DEBUG_FUNC();
    if (!_astMutexes[s16MutexId].bUsed)
    {
        SH_DEBUG_ERR(("mutex %d not be used\n",s16MutexId));
        return FALSE;
    }
    strcpy(stMutex->as8Name,_astMutexes[s16MutexId].as8Name);
    stMutex->stMutex           = _astMutexes[s16MutexId].stMutex;
    stMutex->hOwerThreadHandle = _astMutexes[s16MutexId].hOwerThreadHandle;
    SH_DEBUG_INFO(("as8Name:%s\n",stMutex->as8Name));
    return TRUE;
}

SH_U16  SH_GetOpenMutexNum(void)
{
    SH_U32 u32Index;
    SH_U32 u32MutexNum = 0;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return 0;
    }
    pthread_mutex_lock(&_stMutexMutex);
    for (u32Index = 0;u32Index < SH_MAX_MUTEX_NUM;u32Index++)
    {
        if (_astMutexes[u32Index].bUsed)
        {
            u32MutexNum++;
        }
    }
    pthread_mutex_unlock(&_stMutexMutex);
    SH_DEBUG_INFO(("u32MutexNum = %d\n",(int)u32MutexNum));
    return u32MutexNum;
}

SH_S16  SH_CreateEventGroup(SH_U32 u32InitialState,SH_S8* ps8Name)
{
    SH_U32              u32Index;
    pthread_mutexattr_t stMutex;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return SH_INVALID_MUTEX;
    }

    pthread_mutexattr_init(&stMutex);
    pthread_mutex_lock(&_stEventMutex);
    for (u32Index = 0;u32Index < SH_MAX_EVENT_GROUP;u32Index++)
    {
        if (!_astEvents[u32Index].bUsed)
        {
            pthread_mutex_init(&_astEvents[u32Index].stMutexEvent, &stMutex);
            pthread_mutex_lock(&_astEvents[u32Index].stMutexEvent);
            _astEvents[u32Index].bUsed         = TRUE;
            _astEvents[u32Index].u32EventGroup = 0;
            strcpy(_astEvents[u32Index].as8Name,ps8Name);
            pthread_mutex_unlock(&_astEvents[u32Index].stMutexEvent);
            pthread_mutex_unlock(&_stEventMutex);
            pthread_mutex_init(&_astEvents[u32Index].stMutex, &stMutex);
            pthread_cond_init(&_astEvents[u32Index].stEvent, NULL);
            if (u32InitialState != 0)
            {
                pthread_mutex_lock(&_astEvents[u32Index].stMutexEvent);
                _astEvents[u32Index].u32EventGroup |= u32InitialState;
                pthread_cond_broadcast(&_astEvents[u32Index].stEvent);
                pthread_mutex_unlock(&_astEvents[u32Index].stMutexEvent);
            }
            
            SH_DEBUG_INFO(("u32Index = %d\n",(int)u32Index));
            return u32Index;
        }
    }
    
    pthread_mutex_unlock(&_stEventMutex);
    SH_DEBUG_ERR(("above max event num!\n"));
    return SH_INVALID_EVENT;
}

SH_BOOL SH_DeleteEventGroup(SH_S16 s16EventId)
{
    SH_DEBUG_FUNC();

    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d not be used\n",s16EventId));
        return TRUE;
    }
    
    pthread_cond_destroy(&_astEvents[s16EventId].stEvent);
    pthread_mutex_destroy(&_astEvents[s16EventId].stMutex);
    pthread_mutex_lock(&_stEventMutex);
    pthread_mutex_lock(&_astEvents[s16EventId].stMutexEvent);
    _astEvents[s16EventId].u32EventGroup = 0;
    pthread_mutex_unlock(&_astEvents[s16EventId].stMutexEvent);
    _astEvents[s16EventId].bUsed = FALSE;
    pthread_mutex_unlock(&_stEventMutex);
    pthread_mutex_destroy(&_astEvents[s16EventId].stMutexEvent);
    return TRUE;
}

SH_BOOL SH_WaitEvent(SH_S16 s16EventId,EWaitMode eWaitMode,SH_U32 u32WaitEventFlag)
{
    SH_BOOL bRet;
    SH_BOOL bAnd;
    SH_BOOL bClear;
    SH_U32  u32RetrievedEventFlag;
    
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d not be used\n",s16EventId));
        return FALSE;
    }

    if (!u32WaitEventFlag)
    {
        return FALSE;
    }

    bClear = ((E_MODE_AND_CLEAR == eWaitMode) || (E_MODE_OR_CLEAR == eWaitMode)) ? TRUE : FALSE;
    bAnd = ((E_MODE_AND == eWaitMode)|| (E_MODE_AND_CLEAR == eWaitMode)) ? TRUE : FALSE;
    pthread_mutex_lock(&_astEvents[s16EventId].stMutexEvent);
    do{
        u32RetrievedEventFlag = _astEvents[s16EventId].u32EventGroup & u32WaitEventFlag;
        if ((bAnd) ? (u32RetrievedEventFlag == u32WaitEventFlag) : (0 != u32RetrievedEventFlag))
        {
            break;
        }
        
        pthread_cond_wait(&_astEvents[s16EventId].stEvent,&_astEvents[s16EventId].stMutexEvent);
    } while (1);

    bRet = (bAnd) ? (u32RetrievedEventFlag == u32WaitEventFlag) : (0 != u32RetrievedEventFlag);
    if (bRet && bClear)
    {
        _astEvents[s16EventId].u32EventGroup &= ~u32RetrievedEventFlag;
    }
    
    pthread_mutex_unlock(&_astEvents[s16EventId].stMutexEvent);
    return bRet;
}

SH_BOOL SH_SetEvent(SH_S16 s16EventId,SH_U32 u32EventFlag)
{
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d not be used\n",s16EventId));
        return FALSE;
    }

    pthread_mutex_lock(&_astEvents[s16EventId].stMutexEvent);
    _astEvents[s16EventId].u32EventGroup |= u32EventFlag;
    pthread_cond_broadcast(&_astEvents[s16EventId].stEvent);
    pthread_mutex_unlock(&_astEvents[s16EventId].stMutexEvent);
    return TRUE;
}

SH_BOOL SH_ClearEvent(SH_S16 s16EventId,SH_U32 u32EventFlag)
{
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d not be used\n",s16EventId));
        return FALSE;
    }

    pthread_mutex_lock(&_astEvents[s16EventId].stMutexEvent);
    _astEvents[s16EventId].u32EventGroup &= ~u32EventFlag;
    pthread_mutex_unlock(&_astEvents[s16EventId].stMutexEvent);
    return TRUE;
}

SH_BOOL SH_EventGetInfo(SH_S16 s16EventId,STEvent *stEvent)
{
    SH_DEBUG_FUNC();
    if (!_astEvents[s16EventId].bUsed)
    {
        SH_DEBUG_WRN(("event %d not be used\n",s16EventId));
        return FALSE;
    }
    
    strcpy(stEvent->as8Name,_astEvents[s16EventId].as8Name);
    stEvent->stEvent = _astEvents[s16EventId].stEvent;
    SH_DEBUG_INFO(("as8Name:%s\n",stEvent->as8Name));
    return TRUE;
}

SH_U32  SH_GetSysTimeMs(void)
{
#if 1
#ifdef clock_gettime
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000+ ts.tv_nsec / 1000000;
#else
    struct timeval tv;

    gettimeofday (&tv, NULL);
    return tv.tv_sec * 1000+ tv.tv_usec / 1000;
#endif
#else
    return clock() / 1000;
#endif
}

SH_U16  SH_GetOpenEventNum(void)
{
    SH_U32 u32Index;
    SH_U32 u32EventNum = 0;
    
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        SH_DEBUG_ERR(("os not init\n"));
        return 0;
    }
    
    pthread_mutex_lock(&_stEventMutex);
    for (u32Index = 0;u32Index < SH_MAX_EVENT_GROUP;u32Index++)
    {
        if (_astEvents[u32Index].bUsed)
        {
            u32EventNum++;
        }
    }
    
    pthread_mutex_unlock(&_stEventMutex);
    SH_DEBUG_INFO(("u32EventNum = %d\n",(int)u32EventNum));
    return u32EventNum;
}

VOID    SH_OS_Init(void)
{
    SH_U32              u32Index;
    pthread_mutexattr_t stAttr;
    
    SH_DEBUG_FUNC();
    if (_bOsInit)
    {
        return;
    }

    _bOsInit = TRUE;
    memset(_astThreads,0,sizeof(_astThreads));
    memset(_astMutexes,0,sizeof(_astMutexes));
    memset(_astEvents,0,sizeof(_astEvents));
    for (u32Index = 0;u32Index < SH_MAX_THREAD_NUM;u32Index++)
    {
        _astThreads[u32Index].hThread           = (HANDLE)SH_INVALID_THREAD;
    }
    
    for (u32Index = 0;u32Index < SH_MAX_MUTEX_NUM;u32Index++)
    {
        _astMutexes[u32Index].hOwerThreadHandle = (HANDLE)SH_INVALID_THREAD;
    }
        
    pthread_mutexattr_init(&stAttr);
    pthread_mutex_init(&_stThreadMutex, &stAttr);
    pthread_mutex_init(&_stMutexMutex, &stAttr);
    pthread_mutex_init(&_stEventMutex, &stAttr);
    pthread_mutex_init(&_stMemMutex, &stAttr);
    
    SH_SetVersion(0,19,LSTRING"os");
    srand(clock());
}

VOID    SH_OS_DeInit(void)
{
    SH_DEBUG_FUNC();
    if (!_bOsInit)
    {
        return;
    }

    pthread_mutex_destroy(&_stThreadMutex);
    pthread_mutex_destroy(&_stMutexMutex);
    pthread_mutex_destroy(&_stEventMutex);
    pthread_mutex_destroy(&_stMemMutex);
    _bOsInit = FALSE;
}
#endif

SH_U32  SH_Rand(void)
{
    static SH_U32 u32PreClock = 0;

    if (clock() - u32PreClock > 500)
    {
        u32PreClock = clock();
        srand(u32PreClock);
    }
    
    return rand();
}

#if SH_DEBUG
VOID    SH_OsSetDebugLevel(SH_U16 u16Level)
{
    _u16OsDebugLevel = u16Level;
}
#endif

SH_U32  SH_GetMallocTimes(void)
{
    return _u32MemAllocedTimes;
}

#endif


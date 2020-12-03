//2015/10/22 V1.1 modify interface
//2015/10/24 V1.2 改成动态库版本
//2016/07/07 V1.3
//对申请到的内存先清零，防止存储字符串出现没有终止符问题
//2017/08/08 V1.4
//增加接口SH_DumpData
//2017/12/25 V1.5
//增加数组排序及查找接口
//2018/01/04 V1.6
//1.修改排序接口参数类型
//2.实现快速排序算法
//2018/01/09 V1.7
//修改排序时比较参数的数据类型
//2018/01/14 V1.8
//优化选择排序算法
//2018/01/16 V1.9
//修改dump 数据接口的兼容性
//2018/07/06 V1.10
//增加64位版本支持
//2018/11/19 V1.11
//增加堆排序算法
//2019/01/03 V1.12
//增加归并排序算法
//2020/05/23 V1.13
//增加二进制查看接口SH_HexDump
//2020/06/12 V1.14
//优化排序算法的数据交换过程，减少内存分配释放次数
#include "config.h"
#if ((SH_LINK == sh_enable) || (SH_QUEUE == sh_enable) || (SH_STACK == sh_enable))

#ifndef _SH_DATA_DLL_
#define _SH_DATA_DLL_
#endif

#include "sh_data.h"
#include "sh_commoninteral.h"
#include "sh_version.h"
#include "sh_os.h"

#define SH_DEBUG_LEVEL _u16DataDbgLev

#if SH_DEBUG
static SH_U16 _u16DataDbgLev = SH_DEBUGLEVEL_ERR;
#endif

static SHMemFree        _pMemFree     = NULL;
static SHMemAllocate    _pMemAlloc    = NULL;
static SHMemoryPoolInfo _pMemPoolInfo = NULL;
static STMutexFun       _pstMutexFunc = {NULL,NULL,NULL,NULL};

void sh_MemFree(void* pAddress)
{
    if(_pMemFree)
        _pMemFree(pAddress);
}

void* sh_MemAllocate(SH_U32 u32Size)
{
    void* pAddr;
    
    if(_pMemAlloc)
    {
        pAddr = _pMemAlloc(u32Size);
        memset(pAddr,0,u32Size);//对申请到的内存先清零，防止存储字符串出现没有终止符问题
        return pAddr;
    }
    return NULL;
}
SH_BOOL sh_MemoryPoolInfo(void** pPoolAddr,
                               SH_U32* pu32PoolSize,
                               SH_U32* pu32FreeSize,
                               SH_U32* pu32LargestFreeBlockSize)
{
    if(_pMemPoolInfo)
    {
        _pMemPoolInfo(pPoolAddr,pu32PoolSize,pu32FreeSize,pu32LargestFreeBlockSize);
        return TRUE;
    }
    return FALSE;
}

SH_S16 sh_DataCreateMutex(void)
{
    if(_pstMutexFunc.pMutexCreate)
        return _pstMutexFunc.pMutexCreate();
    else
        return -1;
}

void sh_DataDeleteMutex(SH_S16 s16MutexId)
{
    if(_pstMutexFunc.pMutexDelete)
        _pstMutexFunc.pMutexDelete(s16MutexId);
}

void sh_DataLock(SH_S16 s16MutexId)
{
    if(_pstMutexFunc.pMutexLock)
        _pstMutexFunc.pMutexLock(s16MutexId);
}

void sh_DataUnlock(SH_S16 s16MutexId)
{
    if(_pstMutexFunc.pMutexUnLock)
        _pstMutexFunc.pMutexUnLock(s16MutexId);
}

void sh_DataSetMemFun(SHMemFree pMemFree,SHMemAllocate pMemAlloc,SHMemoryPoolInfo pMemPoolInfo)
{
    _pMemFree     = pMemFree;
    _pMemAlloc    = pMemAlloc;
    _pMemPoolInfo = pMemPoolInfo;

    sh_SetVersion(1,14,LSTRING"link");
    sh_SetVersion(1,5,LSTRING"queue");
    sh_SetVersion(2,1,LSTRING"stack");
    sh_SetVersion(1,13,LSTRING"data");
}

SH_S8 sh_DataCompare(void* pData1,void* pData2,EDATATYPE eType)
{
    switch(eType)
    {
        case E_U8:
            if (*((SH_U8*)pData1) > *((SH_U8*)pData2))
            {
                return 1;
            }
            else if (*((SH_U8*)pData1) < *((SH_U8*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        case E_S8:
            if (*((SH_S8*)pData1) > *((SH_S8*)pData2))
            {
                return 1;
            }
            else if (*((SH_S8*)pData1) < *((SH_S8*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        case E_U16:
            if (*((SH_U16*)pData1) > *((SH_U16*)pData2))
            {
                return 1;
            }
            else if (*((SH_U16*)pData1) < *((SH_U16*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        case E_S16:
            if (*((SH_S16*)pData1) > *((SH_S16*)pData2))
            {
                return 1;
            }
            else if (*((SH_S16*)pData1) < *((SH_S16*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        case E_U32:
            if (*((SH_U32*)pData1) > *((SH_U32*)pData2))
            {
                return 1;
            }
            else if (*((SH_U32*)pData1) < *((SH_U32*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        case E_S32:
            if (*((SH_S32*)pData1) > *((SH_S32*)pData2))
            {
                return 1;
            }
            else if (*((SH_S32*)pData1) < *((SH_S32*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        case E_FLOAT:
            if (*((FLOAT*)pData1) > *((FLOAT*)pData2))
            {
                return 1;
            }
            else if (*((FLOAT*)pData1) < *((FLOAT*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        default:
            if (*((DOUBLE*)pData1) > *((DOUBLE*)pData2))
            {
                return 1;
            }
            else if (*((DOUBLE*)pData1) < *((DOUBLE*)pData2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
    }
}

void sh_DataGetMax(void* pData,EDATATYPE eType)
{
    switch(eType)
    {
        case E_U8:
            *((SH_U8*)pData) = 0xff;
            break;
        case E_S8:
            *((SH_S8*)pData) = 0x7f;
            break;
        case E_U16:
            *((SH_U16*)pData) = 0xffff;
            break;
        case E_S16:
            *((SH_S16*)pData) = 0x7fff;
            break;
        case E_U32:
            *((SH_U32*)pData) = 0xffffffff;
            break;
        case E_S32:
            *((SH_S32*)pData) = 0x7fffffff;
            break;
        case E_FLOAT:
            *((FLOAT*)pData) = 0xff;
            break;
        default:
            *((DOUBLE*)pData) = 0xff;
            break;
    }
}

void sh_DataGetMin(void* pData,EDATATYPE eType)
{
    switch(eType)
    {
        case E_U8:
            *((SH_U8*)pData) = 0x0;
            break;
        case E_S8:
            *((SH_S8*)pData) = (SH_S8)0x80;
            break;
        case E_U16:
            *((SH_U16*)pData) = 0x0;
            break;
        case E_S16:
            *((SH_S16*)pData) = (SH_S16)0x8000;
            break;
        case E_U32:
            *((SH_U32*)pData) = 0x0;
            break;
        case E_S32:
            *((SH_S32*)pData) = 0x80000000;
            break;
        case E_FLOAT:
            *((FLOAT*)pData) = 0xff;
            break;
        default:
            *((DOUBLE*)pData) = 0xff;
            break;
    }
}

SH_U8 sh_DataGetTypeSize(EDATATYPE eType)
{
    switch(eType)
    {
        case E_U8:
        case E_S8:
            return 1;
        case E_U16:
        case E_S16:
            return 2;
        case E_U32:
        case E_S32:
            return 4;
        case E_FLOAT:
            return 4;
        default:
            return 8;
    }
}

void SH_DataSetMemFun(SHMemFree pMemFree,SHMemAllocate pMemAlloc,SHMemoryPoolInfo pMemPoolInfo)
{
    sh_DataSetMemFun(pMemFree,pMemAlloc,pMemPoolInfo);
}

void SH_DataSetMutexFun(STMutexFun* pstMutexFun)
{
    _pstMutexFunc.pMutexCreate = pstMutexFun->pMutexCreate;
    _pstMutexFunc.pMutexDelete = pstMutexFun->pMutexDelete;
    _pstMutexFunc.pMutexLock   = pstMutexFun->pMutexLock;
    _pstMutexFunc.pMutexUnLock = pstMutexFun->pMutexUnLock;
}

void SH_DumpData(void* pData,SH_U32 u32Num,EDATATYPE eType,SH_S8* ps8Format,SH_U8 u8RowNum)
{
    SH_U32 u32Idx;

    if (pData == NULL)
    {
        return;
    }
    
    for (u32Idx = 0;u32Idx < u32Num;u32Idx++)
    {
        switch(eType)
        {
            case E_U8:
                printf(ps8Format,*((SH_U8*)pData + u32Idx));
                break;
            case E_S8:
                printf(ps8Format,*((SH_S8*)pData + u32Idx));
                break;
            case E_U16:
                printf(ps8Format,*((SH_U16*)pData + u32Idx));
                break;
            case E_S16:
                printf(ps8Format,*((SH_S16*)pData + u32Idx));
                break;
            case E_U32:
                printf(ps8Format,*((SH_U32*)pData + u32Idx));
                break;
            case E_S32:
                printf(ps8Format,*((SH_S32*)pData + u32Idx));
                break;
            case E_FLOAT:
                printf(ps8Format,*((FLOAT*)pData + u32Idx));
                break;
            default:
                printf(ps8Format,*((DOUBLE*)pData + u32Idx));
                break;
        }
        
        if (u32Idx % u8RowNum == (SH_U8)(u8RowNum - 1))
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }

    printf("\n");
}

void SH_DumpDataS32(SH_S32* ps32Data,SH_U32 u32Num,SH_U8 u8RowNum)
{
    SH_U32 u32Idx;

    if (ps32Data == NULL)
    {
        return;
    }
    
    for (u32Idx = 0;u32Idx < u32Num;u32Idx++)
    {
        printf("%d",ps32Data[u32Idx]);
        if (u32Idx % u8RowNum == (SH_U8)(u8RowNum - 1))
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }

    printf("\n");
}

void SH_HexDump(void* pData,SH_S32 s32Size)
{
    SH_S8 s8Byte;
    SH_S32 s32Idx;
    SH_S32 s32Idy;
    SH_S8 as8Str[0x1000];
    SH_S32 s32LineNum = 0;
    SH_S32 s32Len;

    s32Len = 0;
    for (s32Idx = 0; s32Idx < s32Size; s32Idx++)
    {
        s8Byte = ((SH_S8*)pData)[s32Idx];
        if (s32Idx % 16 == 0)
        {
            s32Len += sprintf(as8Str + s32Len,"%08x|",s32LineNum);
        }
        
        s32Len += sprintf(as8Str + s32Len,"%02x ",((SH_S8*)pData)[s32Idx]);
        if ((s32Idx % 16 == 15) || (s32Idx == s32Size -1))
        {
            for (s32Idy = 0; s32Idy < 15 -(s32Idx % 16); s32Idy++)
            {
                s32Len += sprintf(as8Str + s32Len,"   ");
            }
            
            s32Len += sprintf(as8Str + s32Len,"|");
            for (s32Idy = (s32Idx - (s32Idx % 16)); s32Idy <= s32Idx; s32Idy++)
            {
                s8Byte = ((SH_S8*)pData)[s32Idy];
                if (s8Byte > 31 && s8Byte < 127)
                {
                    s32Len += sprintf(as8Str + s32Len,"%c", s8Byte);
                }
                else
                {
                    s32Len += sprintf(as8Str + s32Len,".");
                }
            }
            
            s32Len += sprintf(as8Str + s32Len,"\n");
            s32LineNum++;
            if (s32Len + 0x80 > 0x1000)
            {
                printf("warning!!!above 4k!!!\n");
                break;
            }
        }
    }

    printf("%s",as8Str);
}

static void _sh_Swap(void* pData1,void* pData2,void* pData,SH_U32 u32DataSize)
{
    memcpy(pData,pData1,u32DataSize);
    memcpy(pData1,pData2,u32DataSize);
    memcpy(pData2,pData,u32DataSize);
}

EERRCODE SH_SortUpArraySel(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    SH_U32 u32Idx;
    SH_U32 u32Idy;
    SH_U32 u32MinIdx;
    void*  pMin;
    void*  pDataTmp;

    CHECKPOINTER(pData);
    pMin = sh_MemAllocate(u32DataSize);
    pDataTmp = sh_MemAllocate(u32DataSize);
    for (u32Idx = 0;u32Idx < u32DataNum - 1;u32Idx++)
    {
        memcpy(pMin,(void*)((SH_ADDR)pData + u32DataSize * u32Idx),u32DataSize);
        u32MinIdx = u32Idx;
        for (u32Idy = u32Idx;u32Idy < u32DataNum;u32Idy++)
        {
            if (sh_DataCompare((void*)((SH_ADDR)pData + u32DataSize * u32Idy
                + u32ComOff),(SH_U8*)pMin + u32ComOff,eType) < 0)
            {
                u32MinIdx = u32Idy;
                memcpy(pMin,(void*)((SH_ADDR)pData + u32DataSize * u32Idy),u32DataSize);
            }
        }

        _sh_Swap((SH_U8*)pData + u32DataSize * u32Idx,
            (SH_U8*)pData + u32DataSize * u32MinIdx,pDataTmp,u32DataSize);
    }

    sh_MemFree(pDataTmp);
    sh_MemFree(pMin);
    return E_NOERR;
}

EERRCODE SH_SortUpArrayBubble(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    SH_U32  u32Idx;
    SH_BOOL bSwap = FALSE;
    void*   pDataTmp;

    CHECKPOINTER(pData);
    pDataTmp = sh_MemAllocate(u32DataSize);
    while (u32DataNum--)
    {
        for (u32Idx = 0;u32Idx < u32DataNum;u32Idx++)
        {
            if (sh_DataCompare((void*)((SH_ADDR)pData + u32DataSize * u32Idx + u32ComOff),
                (void*)((SH_ADDR)pData + u32DataSize * (u32Idx + 1) + u32ComOff),eType) > 0)
            {
                bSwap = TRUE;
                _sh_Swap((SH_U8*)pData + u32DataSize * u32Idx,
                    (SH_U8*)pData + u32DataSize * (u32Idx + 1),pDataTmp,u32DataSize);
            }
        }

        if (bSwap == FALSE)
        {
            break;
        }
    }

    sh_MemFree(pDataTmp);
    return E_NOERR;
}

SH_BOOL sh_SortCompare(void* pData1,void* pData2,EDATATYPE eType,SH_BOOL bBig)
{
    if (bBig)
    {
        return sh_DataCompare(pData1,pData2,eType) >= 0;
    }

    return sh_DataCompare(pData1,pData2,eType) <= 0;
}

static EERRCODE _sh_SortFast(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32ComOff,SH_S32 s32St,SH_S32 s32En,SH_BOOL bUp)
{
    #if 1
    SH_S32 s32Start = s32St;
    SH_S32 s32End = s32En;
    void*  pTmp;

    pTmp = sh_MemAllocate(u32DataSize);
    memcpy(pTmp,(void*)((SH_ADDR)pData + u32DataSize * s32St),u32DataSize);
    while (s32Start < s32End)
    {
        while (sh_SortCompare((void*)((SH_ADDR)pData + u32DataSize * s32End
            + u32ComOff),(SH_U8*)pTmp + u32ComOff,eType,bUp) && (s32End > s32Start))
        {
            s32End--;
        }
        
        if (s32End > s32Start)
        {
            memcpy((void*)((SH_ADDR)pData + u32DataSize * s32Start),
                (void*)((SH_ADDR)pData + u32DataSize * s32End),u32DataSize);
            s32Start++;
            while (sh_SortCompare((void*)((SH_ADDR)pData + u32DataSize * s32Start
                + u32ComOff),(SH_U8*)pTmp + u32ComOff,eType,!bUp) && (s32Start < s32End))
            {
                s32Start++;
            }

            if (s32Start < s32End)
            {
                memcpy((SH_S32*)((SH_ADDR)pData + u32DataSize * s32End),
                    (SH_S32*)((SH_ADDR)pData + u32DataSize * s32Start),u32DataSize);
                s32End--;
            }
        }
    }

    memcpy((void*)((SH_ADDR)pData + u32DataSize * s32Start),pTmp,u32DataSize);
    sh_MemFree(pTmp);
    if(s32St < s32Start - 1)
    {
        _sh_SortFast(pData,u32DataSize,eType,u32ComOff,s32St,s32Start - 1,bUp);
    }
    
    if(s32End + 1 < s32En)
    {
        _sh_SortFast(pData,u32DataSize,eType,u32ComOff,s32End + 1,s32En,bUp);
    }
    #else
    SH_S32 s32Idx;
    SH_S32 s32Idy;

    if (s32St >= s32En)
    {
        return E_NOERR;
    }
    
    s32Idy = s32St;
    for (s32Idx = s32St;s32Idx < s32En;s32Idx++)
    {
        if (sh_SortCompare((SH_U8*)pData + u32DataSize * s32Idx
            + u32ComOff,(SH_U8*)pData + u32DataSize * s32En + u32ComOff,eType,!bUp))
        {
            _sh_Swap((SH_U8*)pData + u32DataSize * s32Idy,
                (SH_U8*)pData + u32DataSize * s32Idx,u32DataSize);
            s32Idy++;
        }
    }

     _sh_Swap((SH_U8*)pData + u32DataSize * s32Idy,
        (SH_U8*)pData + u32DataSize * s32En,u32DataSize);
    _sh_SortFast(pData,u32DataSize,eType,u32ComOff,s32St,s32Idy - 1,bUp);
    _sh_SortFast(pData,u32DataSize,eType,u32ComOff,s32Idy + 1,s32En,bUp);
    #endif
    return E_NOERR;
}

EERRCODE SH_SortUpArrayFast(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    CHECKPOINTER(pData);
    return _sh_SortFast(pData,u32DataSize,eType,u32ComOff,0,u32DataNum - 1,TRUE);
}

EERRCODE SH_SortDownArraySel(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    SH_U32 u32Idx;
    SH_U32 u32Idy;
    SH_U32 u32MaxIdx;
    void*  pMax;
    void*  pDataTmp;

    CHECKPOINTER(pData);
    pMax = sh_MemAllocate(u32DataSize);
    pDataTmp = sh_MemAllocate(u32DataSize);
    for (u32Idx = 0;u32Idx < u32DataNum - 1;u32Idx++)
    {
        memcpy(pMax,(void*)((SH_ADDR)pData + u32DataSize * u32Idx),u32DataSize);
        u32MaxIdx = u32Idx;
        for (u32Idy = u32Idx;u32Idy < u32DataNum;u32Idy++)
        {
            if (sh_DataCompare((void*)((SH_ADDR)pData + u32DataSize * u32Idy
                + u32ComOff),(SH_U8*)pMax + u32ComOff,eType) > 0)
            {
                u32MaxIdx = u32Idy;
                memcpy(pMax,(void*)((SH_ADDR)pData + u32DataSize * u32Idy),u32DataSize);
            }
        }

        _sh_Swap((SH_U8*)pData + u32DataSize * u32Idx,
            (SH_U8*)pData + u32DataSize * u32MaxIdx,pDataTmp,u32DataSize);
    }

    sh_MemFree(pDataTmp);
    sh_MemFree(pMax);
    return E_NOERR;
}

EERRCODE SH_SortDownArrayBubble(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    SH_U32  u32Idx;
    SH_U32  u32Idy;
    SH_BOOL bSwap = FALSE;
    void*   pDataTmp;

    CHECKPOINTER(pData);
    pDataTmp = sh_MemAllocate(u32DataSize);
    for (u32Idx = 0;u32Idx < u32DataNum - 1;u32Idx++)
    {
        for (u32Idy = u32DataNum - 1;u32Idy > u32Idx;u32Idy--)
        {
            if (sh_DataCompare((void*)((SH_ADDR)pData + u32DataSize * u32Idy + u32ComOff),
                (void*)((SH_ADDR)pData + u32DataSize * (u32Idy - 1) + u32ComOff),eType) > 0)
            {
                bSwap = TRUE;
                _sh_Swap((SH_U8*)pData + u32DataSize * u32Idy,
                    (SH_U8*)pData + u32DataSize * (u32Idy - 1),pDataTmp,u32DataSize);
            }
        }

        if (bSwap == FALSE)
        {
            break;
        }
    }

    sh_MemFree(pDataTmp);
    return E_NOERR;
}

EERRCODE SH_SortDownArrayFast(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    CHECKPOINTER(pData);
    return _sh_SortFast(pData,u32DataSize,eType,u32ComOff,0,u32DataNum - 1,FALSE);
}

static void _sh_Heap(void* pData,void* pDataTmp,SH_U32 u32DataSize,SH_U32 u32Idx,
    EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff,SH_BOOL bMax)
{
    SH_U32 u32Left;
    SH_U32 u32Right;
    SH_U32 u32KeyIdx;
    SH_S8  s8Flag;

    u32Left  = u32Idx << 1;
    u32Right = (u32Idx << 1) | 0x1;
    s8Flag   = bMax ? 1 : -1;
    if ((u32Left <= u32DataNum) && (sh_DataCompare((void*)((SH_ADDR)pData + u32DataSize * (u32Left - 1) + u32ComOff),
        (void*)((SH_ADDR)pData + u32DataSize * (u32Idx - 1) + u32ComOff),eType) == s8Flag))
    {
        u32KeyIdx = u32Left;
    }
    else
    {
        u32KeyIdx = u32Idx;
    }

    if ((u32Right <= u32DataNum) && (sh_DataCompare((void*)((SH_ADDR)pData + u32DataSize * (u32Right - 1) + u32ComOff),
        (void*)((SH_ADDR)pData + u32DataSize * (u32KeyIdx - 1) + u32ComOff),eType) == s8Flag))
    {
        u32KeyIdx = u32Right;
    }

    if (u32KeyIdx != u32Idx)
    {
        _sh_Swap((SH_U8*)pData + u32DataSize * (u32Idx - 1),
            (SH_U8*)pData + u32DataSize * (u32KeyIdx - 1),pDataTmp,u32DataSize);
        _sh_Heap(pData,pDataTmp,u32DataSize,u32KeyIdx,eType,u32DataNum,u32ComOff,bMax);
    }
}

static void _sh_BuildHeap(void* pData,void* pDataTmp,SH_U32 u32DataSize,
    EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff,SH_BOOL bMax)
{
    SH_S32 s32Idx;
    
    for (s32Idx = u32DataNum / 2;s32Idx >= 1;s32Idx--)
    {
        _sh_Heap(pData,pDataTmp,u32DataSize,s32Idx,eType,u32DataNum,u32ComOff,bMax);
    }
}

//由于堆索引从1开始，而数组索引从0开始，所以堆排序中在实际操作数组的数据时需要将索引减1
EERRCODE SH_SortUpArrayHeap(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    SH_U32 u32Idx;
    void*  pDataTmp;

    CHECKPOINTER(pData);
    pDataTmp = sh_MemAllocate(u32DataSize);
    _sh_BuildHeap(pData,pDataTmp,u32DataSize,eType,u32DataNum,u32ComOff,TRUE);
    for (u32Idx = u32DataNum;u32Idx >= 2;u32Idx--)
    {
        _sh_Swap((SH_U8*)pData + u32DataSize * (u32Idx - 1),pData,pDataTmp,u32DataSize);
        _sh_Heap(pData,pDataTmp,u32DataSize,1,eType,u32Idx - 1,u32ComOff,TRUE);
    }

    sh_MemFree(pDataTmp);
    return E_NOERR;
}

EERRCODE SH_SortDownArrayHeap(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    SH_U32 u32Idx;
    void*  pDataTmp;

    CHECKPOINTER(pData);
    pDataTmp = sh_MemAllocate(u32DataSize);
    _sh_BuildHeap(pData,pDataTmp,u32DataSize,eType,u32DataNum,u32ComOff,FALSE);
    for (u32Idx = u32DataNum;u32Idx >= 2;u32Idx--)
    {
        _sh_Swap((SH_U8*)pData + u32DataSize * (u32Idx - 1),pData,pDataTmp,u32DataSize);
        _sh_Heap(pData,pDataTmp,u32DataSize,1,eType,u32Idx - 1,u32ComOff,FALSE);
    }

    sh_MemFree(pDataTmp);
    return E_NOERR;
}


static void _sh_Merge(SH_U8* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32ComOff,SH_U32 u32Start,SH_U32 u32Mid,SH_U32 u32End,SH_BOOL bMax)
{
    SH_U32 s32LIdx      = 0;
    SH_U32 s32RIdx      = 0;
    SH_S32 s32DstIdx    = 0;
    SH_U32 u32LeftNum   = u32Mid - u32Start + 1;
    SH_U32 u32RightNum  = u32End - u32Mid;
    SH_U32 u32LeftSize  = u32DataSize * u32LeftNum;
    SH_U32 u32RightSize = u32DataSize * u32RightNum;
    SH_U8* pLeft        = (SH_U8*)sh_MemAllocate(u32LeftSize);
    SH_U8* pRight       = (SH_U8*)sh_MemAllocate(u32RightSize);
    SH_S8  s8Flag       = bMax ? 1 : -1;

    memcpy(pLeft,pData + u32DataSize * u32Start,u32LeftSize);
    memcpy(pRight,pData + u32DataSize * (u32Mid + 1),u32RightSize);
    while ((s32LIdx < u32LeftNum) && (s32RIdx < u32RightNum))
    {
        if (sh_DataCompare((void*)((SH_ADDR)pLeft + u32DataSize * s32LIdx + u32ComOff),
            (void*)((SH_ADDR)pRight + u32DataSize * s32RIdx + u32ComOff),eType) == s8Flag)
        {
            memcpy(pData + u32DataSize * (u32Start + s32DstIdx++),pRight + u32DataSize * s32RIdx++,u32DataSize);
        }
        else
        {
            memcpy(pData + u32DataSize * (u32Start + s32DstIdx++),pLeft + u32DataSize * s32LIdx++,u32DataSize);
        }
    }

    if (s32LIdx == u32LeftNum)
    {
        memcpy(pData + u32DataSize * (u32Start + s32DstIdx),
            pRight + u32DataSize * s32RIdx,u32DataSize * (u32RightNum - s32RIdx));
    }
    else
    {
        memcpy(pData + u32DataSize * (u32Start + s32DstIdx),
            pLeft + u32DataSize * s32LIdx,u32DataSize * (u32LeftNum - s32LIdx));
    }

    sh_MemFree(pLeft);
    sh_MemFree(pRight);
}

static void _sh_SortArrayMerge(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32ComOff,SH_U32 u32Start,SH_U32 u32End,SH_BOOL bMax)
{
    if (u32Start < u32End)
    {
        _sh_SortArrayMerge(pData,u32DataSize,eType,u32ComOff,u32Start,(u32Start + u32End) >> 1,bMax);
        _sh_SortArrayMerge(pData,u32DataSize,eType,u32ComOff,((u32Start + u32End) >> 1) + 1,u32End,bMax);
        _sh_Merge((SH_U8*)pData,u32DataSize,eType,u32ComOff,u32Start,(u32Start + u32End) >> 1,u32End,bMax);
    }
}

EERRCODE SH_SortUpArrayMerge(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    _sh_SortArrayMerge(pData,u32DataSize,eType,u32ComOff,0,u32DataNum - 1,TRUE);
    return E_NOERR;
}

EERRCODE SH_SortDownArrayMerge(void* pData,SH_U32 u32DataSize,EDATATYPE eType,
    SH_U32 u32DataNum,SH_U32 u32ComOff)
{
    _sh_SortArrayMerge(pData,u32DataSize,eType,u32ComOff,0,u32DataNum - 1,FALSE);
    return E_NOERR;
}

static EERRCODE _sh_FindArrayBin(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,
    SH_U32 u32ComOff,void* pKey,void** pFind,SH_U32 s32Start,SH_U32 s32End)
{
    SH_U32 u32Mid;

    if (s32Start >= s32End)
    {
        if (sh_DataCompare(pKey,(void*)((SH_ADDR)pData + u32DataSize * s32Start
            + u32ComOff),eType) == 0)
        {
            *pFind = (SH_S32*)((SH_ADDR)pData + u32DataSize * s32Start);
            return E_NOERR;
        }
        
        return E_OTHER;
    }
    
    u32Mid = (s32Start + s32End) / 2;
    if (sh_DataCompare(pKey,(SH_S32*)((SH_ADDR)pData + u32DataSize * u32Mid
        + u32ComOff),eType) < 0)
    {
        return _sh_FindArrayBin(pData,u32DataSize,eType,u32DataNum,
            u32ComOff,pKey,pFind,s32Start,u32Mid);
    }
    else if (sh_DataCompare(pKey,(SH_S32*)((SH_ADDR)pData + u32DataSize * u32Mid
        + u32ComOff),eType) > 0)
    {
        return _sh_FindArrayBin(pData,u32DataSize,eType,u32DataNum,
            u32ComOff,pKey,pFind,u32Mid + 1,s32End);
    }
    else
    {
        *pFind = (SH_S32*)((SH_ADDR)pData + u32DataSize * u32Mid);
        return E_NOERR;
    }
}

EERRCODE SH_FindArrayBin(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,
    SH_U32 u32ComOff,void* pKey,void** pFind)
{
    CHECKPOINTER(pData);
    CHECKPOINTER(pFind);
    return _sh_FindArrayBin(pData,u32DataSize,eType,u32DataNum,
        u32ComOff,pKey,pFind,0,u32DataNum - 1);
}

#endif


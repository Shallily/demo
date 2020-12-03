//2015/10/22 V1.1 modify interface
//2015/10/24 V1.2 �ĳɶ�̬��汾
//2015/11/13 V1.3 �޸�extern "C"  ��ʽ
//2017/02/28 V1.4
//�޸�Linux �汾
//2017/08/08 V1.5
//���ӽӿ�SH_DumpData
//2017/12/25 V1.6
//�����������򼰲��ҽӿ�
//2018/01/04 V1.7
//�޸�����ӿڲ�������
//2018/01/09 V1.8
//�޸�����ʱ�Ƚϲ�������������
//2018/01/16 V1.9
//�޸�dump ���ݽӿڵļ�����
//2018/11/19 V1.10
//���Ӷ������㷨
//2019/01/03 V1.11
//���ӹ鲢�����㷨
//2020/05/23 V1.12
//���Ӷ����Ʋ鿴�ӿ�SH_HexDump
#include <stdio.h>
#include "sh_type.h"
#include <string.h>

#ifndef _SH_DATA_H_
#define _SH_DATA_H_

#ifdef __cplusplus
extern "C" { 
#endif

#include "sh_common.h"
#if(SH_OS_TYPE == SH_WINDOWS)
#ifdef _SH_DATA_DLL_
#define SHDATAAPI _declspec(dllexport) 
#else
#define SHDATAAPI _declspec(dllimport) 
#endif
#elif(SH_OS_TYPE == SH_LINUX)
#define SHDATAAPI
#endif

typedef void (*SHMemFree)(void* pAddress);
typedef void* (*SHMemAllocate)(SH_U32 u32Size);
typedef SH_BOOL (*SHMemoryPoolInfo)(void** pPoolAddr,
                                  SH_U32* pu32PoolSize,
                                  SH_U32* pu32FreeSize,
                                  SH_U32* pu32LargestFreeBlockSize);
typedef SH_S16 (*MutexCreate)(void);
typedef void (*MutexDelete)(SH_S16 s16MutexId);
typedef void (*MutexLock)(SH_S16 s16MutexId);
typedef void (*MutexUnlock)(SH_S16 s16MutexId);

typedef struct _shmutexfun
{
    MutexCreate pMutexCreate;
    MutexDelete pMutexDelete;
    MutexLock   pMutexLock;
    MutexUnlock pMutexUnLock;
}STMutexFun;

void SHDATAAPI SH_DataSetMemFun(SHMemFree pMemFree,SHMemAllocate pMemAlloc,SHMemoryPoolInfo pMemPoolInfo);
void SHDATAAPI SH_DataSetMutexFun(STMutexFun* pstMutexFun);

#define EMPTY   {printf("empty pointer\t%s\t%d\n",__FILE__,__LINE__);return FALSE;}
#define MEMFAIL {printf("allocate memory fail\t%s\t%d\n",__FILE__,__LINE__);return FALSE;}

void SHDATAAPI SH_DumpData(void* pData,SH_U32 u32Num,EDATATYPE eType,SH_S8* ps8Format,SH_U8 u8RowNum);

void SHDATAAPI SH_DumpDataS32(SH_S32* ps32Data,SH_U32 u32Num,SH_U8 u8RowNum);

void SHDATAAPI SH_HexDump(void* pData,SH_S32 s32Size);

EERRCODE SHDATAAPI SH_SortUpArraySel(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortUpArrayBubble(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortUpArrayFast(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortUpArrayHeap(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortUpArrayMerge(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortDownArraySel(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortDownArrayBubble(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortDownArrayFast(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortDownArrayHeap(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_SortDownArrayMerge(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff);

EERRCODE SHDATAAPI SH_FindArrayBin(void* pData,SH_U32 u32DataSize,EDATATYPE eType,SH_U32 u32DataNum,SH_U32 u32ComOff,void* pKey,void** pFind);

#ifdef __cplusplus
} 
#endif 

#endif


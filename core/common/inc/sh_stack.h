//2015/10/22 V1.1 modify interface and change single stack to double double
//2015/10/24 V1.2 改成动态库版本
//2015/11/13 V1.3 修改extern "C"  方式
//2016/07/10 V2.0 去掉next 及button 指针优化内存及时间
//2017/02/28 V2.1
//修改Linux 版本
#ifndef _SH_STACK_H_
#define _SH_STACK_H_

#include "sh_common.h"
#if(SH_OS_TYPE == SH_WINDOWS)
#ifdef _SH_STACK_DLL_
#define SHSTACKAPI _declspec(dllexport) 
#else
#define SHSTACKAPI _declspec(dllimport) 
#endif
#elif(SH_OS_TYPE == SH_LINUX)
#define SHSTACKAPI
#endif

#include "sh_data.h"

#ifdef __cplusplus
extern "C" { 
#endif

typedef struct nodes
{
    void*         pData;
    SH_U32        u32Size;
    struct nodes* prev;
}STNodeS;

typedef struct
{
    SH_U32   u32Num;
    STNodeS* top;
    SH_S16   s16MutexId;
}STStack;

SH_BOOL SHSTACKAPI SH_StackInit(STStack* pstStack);

SH_BOOL SHSTACKAPI SH_StackDeInit(STStack* pstStack);

SH_BOOL SHSTACKAPI SH_StackPush(STStack* pstStack,void* pData,SH_U32 u32Size);

SH_BOOL SHSTACKAPI SH_StackPop(STStack* pstStack,void* pData);

SH_U32  SHSTACKAPI SH_StackGetNum(STStack* pstStack);

SH_BOOL SHSTACKAPI SH_StackIsEmpty(STStack* pstStack);

SH_BOOL SHSTACKAPI SH_StackGetTop(STStack* pstStack,void* pData);

#ifdef __cplusplus
} 
#endif 

#endif


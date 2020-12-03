//2015/10/22 V1.1 modify interface and change single stack to double double
//2015/10/24 V1.2 改成动态库版本
//2015/11/17 V1.3 修改deinit 后pstLink->u32Num 变为-1 带来死机风险
//2016/07/10 V2.0 去掉next 及button 指针优化内存及时间
//2016/07/26 V2.1
//分配节点数据内存失败返回时需要释放掉节点内存
#include "config.h"
#if (SH_STACK == sh_enable)

#ifndef _SH_STACK_DLL_
#define _SH_STACK_DLL_
#endif

#include "sh_stack.h"
#include "sh_commoninteral.h"

SH_BOOL SH_StackInit(STStack* pstStack)
{
    if(pstStack == NULL)
        EMPTY;
    pstStack->top        = NULL;
    pstStack->u32Num     = 0;
    pstStack->s16MutexId = sh_DataCreateMutex();
    if(pstStack->s16MutexId == -1)
        return FALSE;
    return TRUE;
}
SH_BOOL SH_StackDeInit(STStack* pstStack)
{
    STNodeS* work;
    
    if(pstStack == NULL)
        EMPTY;
    while(pstStack->u32Num--)
    {
        work          = pstStack->top;
        pstStack->top = work->prev;
        sh_MemFree(work->pData);
        sh_MemFree(work);
    }
    pstStack->u32Num = 0;
    pstStack->top    = NULL;
    sh_DataDeleteMutex(pstStack->s16MutexId);
    return TRUE;
}
SH_BOOL SH_StackPush(STStack* pstStack,void* pData,SH_U32 u32Size)
{
    STNodeS* pstNode;
    
    if((pstStack == NULL)||(pData == NULL))
        EMPTY;
    pstNode          = (STNodeS*)sh_MemAllocate(sizeof(STNodeS));
    if(pstNode == NULL)
        MEMFAIL;
    pstNode->pData   = sh_MemAllocate(u32Size);
    if(pstNode->pData == NULL)
    {
        sh_MemFree(pstNode);
        return FALSE;
    }
    memcpy(pstNode->pData,pData,u32Size);
    sh_DataLock(pstStack->s16MutexId);
    pstNode->prev    = pstStack->top;
    pstNode->u32Size = u32Size;
    pstStack->top    = pstNode;
    pstStack->u32Num++;
    sh_DataUnlock(pstStack->s16MutexId);
    return TRUE;
}
SH_BOOL SH_StackPop(STStack* pstStack,void* pData)
{
    STNodeS* work;
    
    if((pstStack == NULL)||(pData == NULL))
        EMPTY;
    if(pstStack->u32Num == 0)
    {
        return FALSE;
    }
    
    sh_DataLock(pstStack->s16MutexId);
    work = pstStack->top;
    memcpy(pData,work->pData,work->u32Size);
    pstStack->top = work->prev;
    sh_MemFree(work->pData);
    sh_MemFree(work);
    pstStack->u32Num--;
    sh_DataUnlock(pstStack->s16MutexId);
    return TRUE;
}

SH_U32 SH_StackGetNum(STStack* pstStack)
{
    if(pstStack == NULL)
        EMPTY;
    return pstStack->u32Num;
}

SH_BOOL SH_StackIsEmpty(STStack* pstStack)
{
    if(pstStack == NULL)
        EMPTY;
    return (pstStack->u32Num == 0)?1:0;
}

SH_BOOL SH_StackGetTop(STStack* pstStack,void* pData)
{
    if((pstStack == NULL)||(pData == NULL))
        EMPTY;
    if(pstStack->u32Num == 0)
        return FALSE;
    memcpy(pData,pstStack->top->pData,pstStack->top->u32Size);
    return TRUE;
}

#endif


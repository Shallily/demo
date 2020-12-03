//2015/10/22 V1.1 modify interface and change single link to double link
//2015/10/24 V1.2 改成动态库版本
//2015/11/06 V1.3 修改SH_LinkFindNode，pstNode为返回，可以为空；指针非法时应该返回-1
//2015/11/13 V1.4 增加部分内部接口以及SH_LinkDelete 和SH_LinkFind
//2015/11/17 V1.5 修改deinit 后pstLink->u32Num 变为-1 带来死机风险
//2016/07/05 V1.6
//添加接口SH_LinkLock 和SH_LinkUnLock
//2016/07/08 V1.7
//**********************修改SH_LinkDeleteNode 引起链表数据异常重大问题***********************
//2016/07/21 V1.8
//查找算法改成双向查找
//2016/07/26 V1.9
//分配节点数据内存失败返回时需要释放掉节点内存
//2017/12/25 V1.10
//增加链表排序及查找接口
//2018/01/04 V1.11
//实现快速排序算法
//2018/01/09 V1.12
//修改排序时比较参数的数据类型
//2018/01/14 V1.13
//优化选择排序算法
//2018/07/06 V1.14
//增加64位版本支持
#include "config.h"
#if (SH_LINK == sh_enable)

#ifndef _SH_LINK_DLL_
#define _SH_LINK_DLL_
#endif

#include "sh_link.h"
#include "sh_commoninteral.h"

#define SH_DEBUG_LEVEL _u16LinkDbgLev

#if SH_DEBUG
static SH_U16 _u16LinkDbgLev = SH_DEBUGLEVEL_ERR;
#endif

SH_BOOL sh_LinkInit(STLink* pstLink)
{
    if(pstLink == NULL)
        EMPTY;
    pstLink->head       = NULL;
    pstLink->u32Num     = 0;
    pstLink->s16MutexId = sh_DataCreateMutex();
    if(pstLink->s16MutexId == -1)
        return FALSE;
    return TRUE;
}

SH_BOOL sh_LinkDeInit(STLink* pstLink)
{
    STNodeL* pre;
    STNodeL* work;

    if(pstLink == NULL)
        EMPTY;
    work                = pstLink->head;
    while(pstLink->u32Num--)
    {
        pre             = work;
        work            = work->next;
        sh_MemFree(pre->pData);
        sh_MemFree(pre);
    }
    pstLink->u32Num     = 0;
    pstLink->head       = NULL;
    sh_DataDeleteMutex(pstLink->s16MutexId);
    pstLink->s16MutexId = -1;
    return TRUE;
}

SH_BOOL sh_LinkAdd(STLink* pstLink,void* pData,SH_U32 u32Size)
{
    STNodeL* pstNode;

    if((pstLink == NULL)||(pData == NULL))
        EMPTY;
    pstNode          = (STNodeL*)sh_MemAllocate(sizeof(STNodeL));
    if(pstNode == NULL)
        MEMFAIL;
    pstNode->pData   = sh_MemAllocate(u32Size);
    if(pstNode->pData == NULL)
    {
        sh_MemFree(pstNode);
        return FALSE;
    }
    memcpy(pstNode->pData,pData,u32Size);
    pstNode->u32Size = u32Size;
    sh_DataLock(pstLink->s16MutexId);
    if(pstLink->u32Num == 0)
    {
        pstLink->head             = pstNode;
        pstNode->prev             = pstLink->head;
    }
    else
    {
        pstNode->prev             = pstLink->head->prev;
        pstLink->head->prev->next = pstNode;
        pstLink->head->prev       = pstNode;
    }
    pstNode->next                 = pstLink->head;
    pstLink->u32Num++;
    sh_DataUnlock(pstLink->s16MutexId);
    return TRUE;
}

SH_U32 sh_LinkGetNum(STLink* pstLink)
{
    if(pstLink == NULL)
        EMPTY;
    return pstLink->u32Num;
}

SH_BOOL SH_LinkInit(STLink* pstLink)
{
    return sh_LinkInit(pstLink);
}

SH_BOOL SH_LinkDeInit(STLink* pstLink)
{
    return sh_LinkDeInit(pstLink);
}

SH_BOOL SH_LinkAdd(STLink* pstLink,void* pData,SH_U32 u32Size)
{
    return sh_LinkAdd(pstLink,pData,u32Size);
}

SH_U32 SH_LinkGetNum(STLink* pstLink)
{
    return sh_LinkGetNum(pstLink);
}

SH_BOOL SH_LinkIsEmpty(STLink* pstLink)
{
    if(pstLink == NULL)
        EMPTY;
    return (pstLink->u32Num == 0)?1:0;
}

SH_U32 SH_LinkGetMaxSize(STLink* pstLink)
{
    STNodeL* work;
    SH_U32   u32Ret = 0;
    SH_U32   u32Num;
    
    if(pstLink == NULL)
        EMPTY;
    work   = pstLink->head;
    u32Num = pstLink->u32Num;
    while(u32Num--)
    {
        if(u32Ret < work->u32Size)
            u32Ret = work->u32Size;
        work = work->next;
    }
    return u32Ret;
}

SH_BOOL SH_LinkGetFirstNode(STLink* pstLink,STNodeL** pstNode)
{
    if((pstLink == NULL)||(pstNode == NULL))
        EMPTY;
    //sh_DataLock(pstLink->s16MutexId);
    *pstNode = pstLink->head;
    //sh_DataUnlock(pstLink->s16MutexId);
    return TRUE;
}

SH_BOOL SH_LinkDeleteNode(STLink* pstLink,STNodeL* pstNode)
{
    if((pstLink == NULL)||(pstNode == NULL))
        EMPTY;
    //sh_DataLock(pstLink->s16MutexId);
    if (pstNode == pstLink->head)
    {
        pstLink->head = pstLink->head->next;
    }
    pstNode->next->prev = pstNode->prev;
    pstNode->prev->next = pstNode->next;
    sh_MemFree(pstNode->pData);
    sh_MemFree(pstNode);
    pstLink->u32Num--;
    if (pstLink->u32Num == 0)
    {
        pstLink->head = NULL;
    }
    //sh_DataUnlock(pstLink->s16MutexId);
    return TRUE;
}

SH_BOOL SH_LinkDelete(STLink* pstLink,void* pData,SH_U32 u32Size,SH_U32 u32Offset)
{
    STNodeL* workn;
    STNodeL* workp;
    SH_U32   u32Num;
    
    if((pstLink == NULL) || (pData == NULL) || (pstLink->u32Num == 0))
    {
        return FALSE;
    }

    sh_DataLock(pstLink->s16MutexId);
    workn  = pstLink->head;
    workp  = pstLink->head->prev;
    u32Num = (pstLink->u32Num + (pstLink->u32Num) % 2) / 2;
    while(u32Num--)
    {
        //后向查找
        if(memcmp((SH_U8*)(workn->pData) + u32Offset,pData,u32Size) == 0)
        {
            if (workn == pstLink->head)
            {
                pstLink->head = pstLink->head->next;
            }
            workn->next->prev = workn->prev;
            workn->prev->next = workn->next;
            sh_MemFree(workn->pData);
            sh_MemFree(workn);
            pstLink->u32Num--;
            if (pstLink->u32Num == 0)
            {
                pstLink->head = NULL;
            }
            sh_DataUnlock(pstLink->s16MutexId);
            return TRUE;
        }
        workn = workn->next;

        //前向查找
        if(memcmp((SH_U8*)(workp->pData) + u32Offset,pData,u32Size) == 0)
        {
            workp->next->prev = workp->prev;
            workp->prev->next = workp->next;
            sh_MemFree(workp->pData);
            sh_MemFree(workp);
            pstLink->u32Num--;
            sh_DataUnlock(pstLink->s16MutexId);
            return TRUE;
        }
        workp = workp->prev;
    }

    sh_DataUnlock(pstLink->s16MutexId);
    return FALSE;
}


SH_BOOL SH_LinkInsertBeforeNode(STLink* pstLink,STNodeL* pstNode,void* pData,SH_U32 u32Size)
{
    STNodeL* pstNodeInsert;
    
    if((pstLink == NULL)||(pstNode == NULL)||(pData == NULL))
        EMPTY;
    pstNodeInsert          = (STNodeL*)sh_MemAllocate(sizeof(STNodeL));
    if(pstNodeInsert == NULL)
        MEMFAIL;
    pstNodeInsert->pData   = sh_MemAllocate(u32Size);
    if(pstNodeInsert->pData == NULL)
    {
        sh_MemFree(pstNodeInsert);
        return FALSE;
    }
    memcpy(pstNodeInsert->pData,pData,u32Size);
    pstNodeInsert->u32Size = u32Size;
    sh_DataLock(pstLink->s16MutexId);
    pstNodeInsert->next    = pstNode;
    pstNodeInsert->prev    = pstNode->prev;
    pstNode->prev->next    = pstNodeInsert;
    pstNode->prev          = pstNodeInsert;
    pstLink->u32Num++;
    if (pstNode == pstLink->head)
    {
        pstLink->head = pstNodeInsert;
    }
    sh_DataUnlock(pstLink->s16MutexId);
    return TRUE;
}

SH_BOOL SH_LinkInsertAfterNode(STLink* pstLink,STNodeL* pstNode,void* pData,SH_U32 u32Size)
{
    STNodeL* pstNodeInsert;
    
    if((pstLink == NULL)||(pstNode == NULL)||(pData == NULL))
        EMPTY;
    pstNodeInsert          = (STNodeL*)sh_MemAllocate(sizeof(STNodeL));
    if(pstNodeInsert == NULL)
        MEMFAIL;
    pstNodeInsert->pData   = sh_MemAllocate(u32Size);
    if(pstNodeInsert->pData == NULL)
    {
        sh_MemFree(pstNodeInsert);
        return FALSE;
    }
    memcpy(pstNodeInsert->pData,pData,u32Size);
    pstNodeInsert->u32Size = u32Size;
    sh_DataLock(pstLink->s16MutexId);
    pstNodeInsert->prev    = pstNode;
    pstNodeInsert->next    = pstNode->next;
    pstNode->next->prev    = pstNodeInsert;
    pstNode->next          = pstNodeInsert;
    pstLink->u32Num++;
    sh_DataUnlock(pstLink->s16MutexId);
    return TRUE;
}

SH_S32  SH_LinkFindNode(STLink* pstLink,STNodeL** pstNode,void* pData,SH_U32 u32Size,SH_U32 u32Offset)
{
    SH_S32   s32Ret;
    STNodeL* workn;
    STNodeL* workp;
    SH_U32   u32Num;
    
    if((pstLink == NULL) || (pData == NULL) || (pstLink->u32Num == 0))
    {
        *pstNode = NULL;
        return -1;
    }

    //sh_DataLock(pstLink->s16MutexId);
    workn  = pstLink->head;
    workp  = pstLink->head->prev;
    u32Num = (pstLink->u32Num + (pstLink->u32Num) % 2) / 2;
    while(u32Num--)
    {
        //后向查找
        if(memcmp((SH_U8*)(workn->pData) + u32Offset,pData,u32Size) == 0)
        {
            *pstNode = workn;
            s32Ret   = (pstLink->u32Num + (pstLink->u32Num) % 2) / 2 - u32Num - 1;
            //sh_DataUnlock(pstLink->s16MutexId);
            return s32Ret;
        }
        workn = workn->next;

        //前向查找
        if(memcmp((SH_U8*)(workp->pData) + u32Offset,pData,u32Size) == 0)
        {
            *pstNode = workp;
            s32Ret   = (pstLink->u32Num) / 2 + u32Num;
            //sh_DataUnlock(pstLink->s16MutexId);
            return s32Ret;
        }
        workp = workp->prev;
    }
    
    *pstNode = NULL;
    //sh_DataUnlock(pstLink->s16MutexId);
    return -1;
}

SH_S32  SH_LinkFind(STLink* pstLink,void* pDataFind,void* pData,SH_U32 u32Size,SH_U32 u32Offset)
{
    SH_S32   s32Ret;
    STNodeL* workn;
    STNodeL* workp;
    SH_U32   u32Num;
    
    if((pstLink == NULL) || (pDataFind == NULL) || (pData == NULL) || (pstLink->u32Num == 0))
    {
        return -1;
    }

    sh_DataLock(pstLink->s16MutexId);
    workn  = pstLink->head;
    workp  = pstLink->head->prev;
    u32Num = (pstLink->u32Num + (pstLink->u32Num) % 2) / 2;
    while(u32Num--)
    {
        //后向查找
        if(memcmp((SH_U8*)(workn->pData) + u32Offset,pData,u32Size) == 0)
        {
            memcpy(pDataFind,workn->pData,workn->u32Size);
            s32Ret = (pstLink->u32Num + (pstLink->u32Num) % 2) / 2 - u32Num - 1;
            sh_DataUnlock(pstLink->s16MutexId);
            return s32Ret;
        }
        workn = workn->next;

        //前向查找
        if(memcmp((SH_U8*)(workp->pData) + u32Offset,pData,u32Size) == 0)
        {
            memcpy(pDataFind,workp->pData,workp->u32Size);
            s32Ret = (pstLink->u32Num) / 2 + u32Num;
            sh_DataUnlock(pstLink->s16MutexId);
            return s32Ret;
        }
        workp = workp->prev;
    }
    sh_DataUnlock(pstLink->s16MutexId);
    return -1;
}

SH_BOOL SH_LinkLock(STLink* pstLink)
{
    sh_DataLock(pstLink->s16MutexId);
    return TRUE;
}

SH_BOOL SH_LinkUnLock(STLink* pstLink)
{
    sh_DataUnlock(pstLink->s16MutexId);
    return TRUE;
}

static void _sh_SwapLinkNode(STLink* pstLink,STNodeL* pstNode1,STNodeL* pstNode2)
{
    STNodeL stNodeTmp;

    stNodeTmp.pData   = pstNode1->pData;
    stNodeTmp.u32Size = pstNode1->u32Size;
    pstNode1->pData   = pstNode2->pData;
    pstNode1->u32Size = pstNode2->u32Size;
    pstNode2->pData   = stNodeTmp.pData;
    pstNode2->u32Size = stNodeTmp.u32Size;
}

EERRCODE SH_SortUpLinkSel(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType)
{
    SH_U32   u32Num1;
    SH_U32   u32Num2;
    STNodeL* pstNodeWork1;
    STNodeL* pstNodeWork2;
    void*    pMin;
    STNodeL* pstNodeMin;
    
    CHECKPOINTER(pstLink);
    CHECKPOINTER(pstLink->head);
    pMin = sh_MemAllocate(pstLink->head->u32Size);
    u32Num1 = pstLink->u32Num - 1;
    pstNodeWork1 = pstLink->head;
    while (u32Num1--)
    {
        memcpy(pMin,pstNodeWork1->pData,pstNodeWork1->u32Size);
        u32Num2 = u32Num1 + 2;
        pstNodeMin = pstNodeWork2 = pstNodeWork1;
        while (u32Num2--)
        {
            if (sh_DataCompare((void*)((SH_ADDR)(pstNodeWork2->pData)
                + u32ComOff),(SH_U8*)pMin + u32ComOff,eType) < 0)
            {
                pstNodeMin = pstNodeWork2;
                memcpy(pMin,pstNodeWork2->pData,pstNodeWork2->u32Size);
            }

            pstNodeWork2 = pstNodeWork2->next;
        }

        _sh_SwapLinkNode(pstLink,pstNodeWork1,pstNodeMin);
        pstNodeWork1 = pstNodeWork1->next;
    }

    sh_MemFree(pMin);
    return E_NOERR;
}

static EERRCODE _sh_SortLinkFast(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType,
    STNodeL* pstNodeSt,STNodeL* pstNodeEn,SH_BOOL bUp)
{
    STNodeL* pstNodeStart = pstNodeSt;
    STNodeL* pstNodeEnd = pstNodeEn;
    void*    pTmp = pstNodeSt->pData;
    
    while (pstNodeStart != pstNodeEnd)
    {
        while (sh_SortCompare((void*)((SH_ADDR)(pstNodeEnd->pData) + u32ComOff),
            (void*)((SH_ADDR)pTmp + u32ComOff),eType,bUp) && (pstNodeStart != pstNodeEnd))
        {
            pstNodeEnd = pstNodeEnd->prev;
        }

        if (pstNodeStart != pstNodeEnd)
        {
            pstNodeStart->pData = pstNodeEnd->pData;
            pstNodeStart = pstNodeStart->next;
            while (sh_SortCompare((void*)((SH_ADDR)(pstNodeStart->pData) + u32ComOff),
                (void*)((SH_ADDR)pTmp + u32ComOff),eType,!bUp) && (pstNodeStart != pstNodeEnd))
            {
                pstNodeStart = pstNodeStart->next;
            }

            if (pstNodeStart != pstNodeEnd)
            {
                pstNodeEnd->pData = pstNodeStart->pData;
                pstNodeEnd = pstNodeEnd->prev;
            }
        }
    }

    pstNodeStart->pData = pTmp;    
    if(pstNodeSt != pstNodeStart)
    {
        _sh_SortLinkFast(pstLink,u32ComOff,eType,pstNodeSt,pstNodeStart->prev,bUp);
    }
    
    if(pstNodeEnd != pstNodeEn)
    {
        _sh_SortLinkFast(pstLink,u32ComOff,eType,pstNodeEnd->next,pstNodeEn,bUp);
    }

    return E_NOERR;
}

EERRCODE SH_SortUpLinkFast(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType)
{
    CHECKPOINTER(pstLink);
    CHECKPOINTER(pstLink->head);
    CHECKPOINTER(pstLink->head->prev);
    return _sh_SortLinkFast(pstLink,u32ComOff,eType,pstLink->head,pstLink->head->prev,TRUE);
}

EERRCODE SH_SortDownLinkSel(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType)
{
    SH_U32   u32Num1;
    SH_U32   u32Num2;
    STNodeL* pstNodeWork1;
    STNodeL* pstNodeWork2;
    void*    pMax;
    STNodeL* pstNodeMax;
    
    CHECKPOINTER(pstLink);
    pMax = sh_MemAllocate(pstLink->head->u32Size);
    u32Num1 = pstLink->u32Num - 1;
    pstNodeWork1 = pstLink->head;
    while (u32Num1--)
    {
        memcpy(pMax,pstNodeWork1->pData,pstNodeWork1->u32Size);
        u32Num2 = u32Num1 + 2;
        pstNodeMax = pstNodeWork2 = pstNodeWork1;
        while (u32Num2--)
        {
            if (sh_DataCompare((void*)((SH_ADDR)(pstNodeWork2->pData)
                + u32ComOff),(SH_U8*)pMax + u32ComOff,eType) > 0)
            {
                pstNodeMax = pstNodeWork2;
                memcpy(pMax,pstNodeWork2->pData,pstNodeWork2->u32Size);
            }

            pstNodeWork2 = pstNodeWork2->next;
        }

        _sh_SwapLinkNode(pstLink,pstNodeWork1,pstNodeMax);
        pstNodeWork1 = pstNodeWork1->next;
    }

    sh_MemFree(pMax);
    return E_NOERR;
}

EERRCODE SH_SortDownLinkFast(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType)
{
    CHECKPOINTER(pstLink);
    CHECKPOINTER(pstLink->head);
    CHECKPOINTER(pstLink->head->prev);
    return _sh_SortLinkFast(pstLink,u32ComOff,eType,pstLink->head,pstLink->head->prev,FALSE);
}

EERRCODE SH_SortLinkBubble(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType,SH_BOOL bUp)
{
    SH_U32   u32Idx;
    SH_BOOL  bSwap = FALSE;
    STNodeL* pstNodeWork;
    STNodeL* pstNodeTmp;
    SH_U32   u32Num;

    CHECKPOINTER(pstLink);
    u32Num = pstLink->u32Num;
    while (u32Num--)
    {
        pstNodeWork = pstLink->head;
        for (u32Idx = 0;u32Idx < u32Num;u32Idx++)
        {
            pstNodeTmp = pstNodeWork->next;
            if (sh_SortCompare((void*)((SH_ADDR)(pstNodeWork->pData) + u32ComOff),
                (void*)((SH_ADDR)(pstNodeTmp->pData) + u32ComOff),eType,bUp))
            {
                bSwap = TRUE;
                _sh_SwapLinkNode(pstLink,pstNodeWork,pstNodeTmp);
            }

            pstNodeWork = pstNodeWork->next;
        }

        if (bSwap == FALSE)
        {
            break;
        }
    }
    
    return E_NOERR;
}

#endif


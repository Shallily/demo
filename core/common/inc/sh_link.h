//2015/10/22 V1.1 modify interface and change single link to double link
//2015/10/24 V1.2 改成动态库版本
//2015/11/13 V1.3 修改extern "C"  方式，增加SH_LinkDelete 和SH_LinkFind
//上层尽量不要使用指针指向链表，以防deinit 后出现指空而死机，
//使用指向链表的指针可以增加链表的灵活性，如果特殊情况下需要使用
//一定要特别小心
//2016/07/05 V1.4
//添加接口SH_LinkLock 和SH_LinkUnLock
//2017/02/28 V1.5
//修改Linux 版本
//2017/12/25 V1.6
//增加链表排序及查找接口
//2018/01/09 V1.7
//修改排序时比较参数的数据类型
#ifndef _SH_LINK_H_
#define _SH_LINK_H_

#include "sh_common.h"
#if(SH_OS_TYPE == SH_WINDOWS)
#ifdef _SH_LINK_DLL_
#define SHLINKAPI _declspec(dllexport) 
#else
#define SHLINKAPI _declspec(dllimport) 
#pragma comment(lib,"SYS.lib")
#endif
#elif(SH_OS_TYPE == SH_LINUX)
#define SHLINKAPI
#endif

#ifdef __cplusplus
extern "C" { 
#endif

typedef struct nodel
{
    void*         pData;
    SH_U32        u32Size;
    struct nodel* prev;
    struct nodel* next;
}STNodeL;

typedef struct
{
    SH_S16   s16MutexId;
    SH_U32   u32Num;
    STNodeL* head;//尽量不直接访问，如果需要访问注意配合SH_LinkLock 和SH_LinkUnLock 加锁
}STLink;

//注意每个链表在反初始化之前只能初始化一次，否则会造成mutex 资源泄露
SH_BOOL SHLINKAPI SH_LinkInit(STLink* pstLink);

//尽量保证在停止访问链表之后再做deinit，否则有概率死机
SH_BOOL SHLINKAPI SH_LinkDeInit(STLink* pstLink);

SH_BOOL SHLINKAPI SH_LinkAdd(STLink* pstLink,void* pData,SH_U32 u32Size);

SH_U32  SHLINKAPI SH_LinkGetNum(STLink* pstLink);

SH_BOOL SHLINKAPI SH_LinkIsEmpty(STLink* pstLink);

SH_U32  SHLINKAPI SH_LinkGetMaxSize(STLink* pstLink);

//尽量不使用此接口，注意配合SH_LinkLock 和SH_LinkUnLock 对pstNode 加锁
SH_BOOL SHLINKAPI SH_LinkGetFirstNode(STLink* pstLink,STNodeL** pstNode);

//尽量不使用此接口，用SH_LinkDelete 替代，注意配合SH_LinkLock 和SH_LinkUnLock 对pstNode 加锁
//注意pstNode 一定是链表中存在的节点，最好是通过SH_LinkGetFirstNode 或SH_LinkFindNode 得到的
SH_BOOL SHLINKAPI SH_LinkDeleteNode(STLink* pstLink,STNodeL* pstNode);

/*****注意匹配关键字pData 部分匹配问题*****/
SH_BOOL SHLINKAPI SH_LinkDelete(STLink* pstLink,void* pData,SH_U32 u32Size,SH_U32 u32Offset);

SH_BOOL SHLINKAPI SH_LinkInsertBeforeNode(STLink* pstLink,STNodeL* pstNode,void* pData,SH_U32 u32Size);

SH_BOOL SHLINKAPI SH_LinkInsertAfterNode(STLink* pstLink,STNodeL* pstNode,void* pData,SH_U32 u32Size);

/*****注意匹配关键字pData 部分匹配问题*****/
//尽量不使用此接口，用SH_LinkFind 替代，注意配合SH_LinkLock 和SH_LinkUnLock 对pstNode 加锁
SH_S32  SHLINKAPI SH_LinkFindNode(STLink* pstLink,STNodeL** pstNode,void* pData,SH_U32 u32Size,SH_U32 u32Offset);

/*****注意匹配关键字pData 部分匹配问题*****/
SH_S32  SHLINKAPI SH_LinkFind(STLink* pstLink,void* pDataFind,void* pData,SH_U32 u32Size,SH_U32 u32Offset);

//只能配合外部访问节点相关接口使用
SH_BOOL SHLINKAPI SH_LinkLock(STLink* pstLink);

//只能配合外部访问节点相关接口使用
SH_BOOL SHLINKAPI SH_LinkUnLock(STLink* pstLink);

EERRCODE SHLINKAPI SH_SortUpLinkSel(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType);

EERRCODE SHLINKAPI SH_SortUpLinkFast(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType);

EERRCODE SHLINKAPI SH_SortDownLinkSel(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType);

EERRCODE SHLINKAPI SH_SortDownLinkFast(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType);

EERRCODE SHLINKAPI SH_SortLinkBubble(STLink* pstLink,SH_U32 u32ComOff,EDATATYPE eType,SH_BOOL bUp);

#ifdef __cplusplus
} 
#endif 

#endif


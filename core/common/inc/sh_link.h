//2015/10/22 V1.1 modify interface and change single link to double link
//2015/10/24 V1.2 �ĳɶ�̬��汾
//2015/11/13 V1.3 �޸�extern "C"  ��ʽ������SH_LinkDelete ��SH_LinkFind
//�ϲ㾡����Ҫʹ��ָ��ָ�������Է�deinit �����ָ�ն�������
//ʹ��ָ�������ָ������������������ԣ���������������Ҫʹ��
//һ��Ҫ�ر�С��
//2016/07/05 V1.4
//��ӽӿ�SH_LinkLock ��SH_LinkUnLock
//2017/02/28 V1.5
//�޸�Linux �汾
//2017/12/25 V1.6
//�����������򼰲��ҽӿ�
//2018/01/09 V1.7
//�޸�����ʱ�Ƚϲ�������������
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
    STNodeL* head;//������ֱ�ӷ��ʣ������Ҫ����ע�����SH_LinkLock ��SH_LinkUnLock ����
}STLink;

//ע��ÿ�������ڷ���ʼ��֮ǰֻ�ܳ�ʼ��һ�Σ���������mutex ��Դй¶
SH_BOOL SHLINKAPI SH_LinkInit(STLink* pstLink);

//������֤��ֹͣ��������֮������deinit�������и�������
SH_BOOL SHLINKAPI SH_LinkDeInit(STLink* pstLink);

SH_BOOL SHLINKAPI SH_LinkAdd(STLink* pstLink,void* pData,SH_U32 u32Size);

SH_U32  SHLINKAPI SH_LinkGetNum(STLink* pstLink);

SH_BOOL SHLINKAPI SH_LinkIsEmpty(STLink* pstLink);

SH_U32  SHLINKAPI SH_LinkGetMaxSize(STLink* pstLink);

//������ʹ�ô˽ӿڣ�ע�����SH_LinkLock ��SH_LinkUnLock ��pstNode ����
SH_BOOL SHLINKAPI SH_LinkGetFirstNode(STLink* pstLink,STNodeL** pstNode);

//������ʹ�ô˽ӿڣ���SH_LinkDelete �����ע�����SH_LinkLock ��SH_LinkUnLock ��pstNode ����
//ע��pstNode һ���������д��ڵĽڵ㣬�����ͨ��SH_LinkGetFirstNode ��SH_LinkFindNode �õ���
SH_BOOL SHLINKAPI SH_LinkDeleteNode(STLink* pstLink,STNodeL* pstNode);

/*****ע��ƥ��ؼ���pData ����ƥ������*****/
SH_BOOL SHLINKAPI SH_LinkDelete(STLink* pstLink,void* pData,SH_U32 u32Size,SH_U32 u32Offset);

SH_BOOL SHLINKAPI SH_LinkInsertBeforeNode(STLink* pstLink,STNodeL* pstNode,void* pData,SH_U32 u32Size);

SH_BOOL SHLINKAPI SH_LinkInsertAfterNode(STLink* pstLink,STNodeL* pstNode,void* pData,SH_U32 u32Size);

/*****ע��ƥ��ؼ���pData ����ƥ������*****/
//������ʹ�ô˽ӿڣ���SH_LinkFind �����ע�����SH_LinkLock ��SH_LinkUnLock ��pstNode ����
SH_S32  SHLINKAPI SH_LinkFindNode(STLink* pstLink,STNodeL** pstNode,void* pData,SH_U32 u32Size,SH_U32 u32Offset);

/*****ע��ƥ��ؼ���pData ����ƥ������*****/
SH_S32  SHLINKAPI SH_LinkFind(STLink* pstLink,void* pDataFind,void* pData,SH_U32 u32Size,SH_U32 u32Offset);

//ֻ������ⲿ���ʽڵ���ؽӿ�ʹ��
SH_BOOL SHLINKAPI SH_LinkLock(STLink* pstLink);

//ֻ������ⲿ���ʽڵ���ؽӿ�ʹ��
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


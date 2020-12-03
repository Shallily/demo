#include "sh_demo.h"
#include "sh_os.h"
#include "sh_data.h"

extern "C" void sh_AppInit(void);
extern "C" void sh_AppDeInit(void);

void* data_alloc(SH_U32 u32Size)
{
    return (void*)SH_Malloc(u32Size);
}

void data_free(void *pAddress)
{
    SH_Free(pAddress);
}

SH_BOOL data_mempoolinfo(void      **pPoolAddr,
                          SH_U32    *pu32PoolSize,
                          SH_U32    *pu32FreeSize,
                          SH_U32    *pu32LargestFreeBlockSize)
{
    return TRUE;
}

static SH_S16 _data_createmtx(void)
{
    return SH_CreateMutex(FALSE,LSTRING"data");
}

int main(void)
{
    STMutexFun stMtxFun;
    
    SH_OS_Init();
    SH_DataSetMemFun(data_free,data_alloc,data_mempoolinfo);
    stMtxFun.pMutexCreate = _data_createmtx;
    stMtxFun.pMutexDelete = (MutexDelete)SH_DeleteMutex;
    stMtxFun.pMutexLock   = (MutexLock)SH_Lock;
    stMtxFun.pMutexUnLock = (MutexUnlock)SH_UnLock;
    SH_DataSetMutexFun(&stMtxFun);
    sh_AppInit();
    
    SH_DemoMultiuserStart();
    
    sh_AppDeInit();
    SH_OS_DeInit();
    return 0;
}


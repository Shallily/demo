#ifndef _SH_DEMO_H_
#define _SH_DEMO_H_

#include "sh_common.h"

#if(SH_OS_TYPE == SH_WINDOWS)
#ifdef _SH_DEMO_DALL_
#define SHDEMOAPI _declspec(dllexport) 
#else
#define SHDEMOAPI _declspec(dllimport)
#endif
#elif(SH_OS_TYPE == SH_LINUX)
#define SHDEMOAPI
#endif

#ifdef __cplusplus
extern "C" { 
#endif 

#define SH_MAX_CMD_NAME 32

typedef SH_BOOL(*SHFunc)(
    void* pInput0,
    void* pInput1,
    void* pInput2,
    void* pInput3,
    void* pInput4,
    void* pInput5
);

typedef struct _DemoFun
{
    SH_S8   s8CmdName[SH_MAX_CMD_NAME];
    SH_U8   u8Level;//for multi-user system
    SH_U8   bLocalorRemote;//for multi-user system 1 for local,2 for remote,3 for both
    SH_U8   u8InputMask; 
    SH_BOOL(*SHFunc)(
        void* pInput0,
        void* pInput1,
        void* pInput2,
        void* pInput3,
        void* pInput4,
        void* pInput5
    );
    SH_S8*  ps8Help;
}STDemoFun;

void   SHDEMOAPI SH_DemoMultiuserStart(void);

void   SHDEMOAPI SH_DemoMultiuserInsertCmd(STDemoFun* pstDemoFun);

DOUBLE SHDEMOAPI SH_DemoMultiuserStrtoNum(SH_S8* ps8Str);

#ifdef __cplusplus
} 
#endif 

#endif


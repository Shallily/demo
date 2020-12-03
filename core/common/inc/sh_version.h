//2017/05/15 V0.1
//2019/10/14 V0.2
//º”¥ÛMAX_MODULE_NUM
#ifndef _SH_VERSION_H_
#define _SH_VERSION_H_

#include "sh_common.h"
#if(SH_OS_TYPE == SH_WINDOWS)
#ifdef _SH_VERSION_DLL_
#define SHVERSIONAPI _declspec(dllexport) 
#else
#define SHVERSIONAPI _declspec(dllimport)
#pragma comment(lib,"SYS.lib")
#endif
#else
#define SHVERSIONAPI
#endif

#ifdef __cplusplus
extern "C" { 
#endif

#define MAX_MODULE_NUM  (0x100)
#define MAX_MODULE_NAME (0x10)

typedef struct
{
    SH_U32 u32Pre;
    SH_U32 u32Last;
    SH_S8  as8ModuleName[MAX_MODULE_NAME];
}STVer;

SH_BOOL SHVERSIONAPI SH_SetVersion(SH_U32 u32Pre,SH_U32 u32Last,SH_S8* ps8Module);

SH_BOOL SHVERSIONAPI SH_GetAllVersion(STVer** pstVer,SH_U32* pu32Num);

#ifdef __cplusplus
} 
#endif 

#endif


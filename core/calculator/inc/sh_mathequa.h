//2017/06/14 V0.1
//2018/01/12 V0.2
//增加多参数支持，最多可以支持3 个参数，分别用x 、y、z
#ifndef _SH_MATHEQUA_H_
#define _SH_MATHEQUA_H_

#include "sh_common.h"
#if(SH_OS_TYPE == SH_WINDOWS)
#ifdef _SH_MATHEQUA_DLL_
#define SHMATHEQUAAPI _declspec(dllexport) 
#else
#define SHMATHEQUAAPI _declspec(dllimport)
#pragma comment(lib,"SHTOOL.lib")
#endif
#else
#define SHMATHEQUAAPI
#endif

#ifdef __cplusplus
extern "C" { 
#endif

EERRCODE SHMATHEQUAAPI SH_MathEqua(SH_S8* ps8Equa,DOUBLE* pdInput,DOUBLE* pdOut);

VOID     SHMATHEQUAAPI SH_MathEquaVersion(void);

#ifdef __cplusplus
} 
#endif 

#endif


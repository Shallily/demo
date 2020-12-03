//2017/05/18 V0.1
#ifndef _SH_VERSION_DLL_
#define _SH_VERSION_DLL_
#endif

#include <string.h>
#include "sh_commoninteral.h"
#include "sh_version.h"

static SH_U32 _u32VerNum = 0;
static STVer  _astVer[MAX_MODULE_NUM];

SH_BOOL sh_SetVersion(SH_U32 u32Pre,SH_U32 u32Last,SH_S8* ps8Module)
{
    SH_U32 u32VerNum;
    
    if (ps8Module == NULL)
    {
        return FALSE;
    }

    for (u32VerNum = 0;u32VerNum < _u32VerNum;u32VerNum++)
    {
        if (strcmp(_astVer[u32VerNum].as8ModuleName,ps8Module) == 0)
        {
            return TRUE;
        }
    }

    strcpy(_astVer[_u32VerNum].as8ModuleName,ps8Module);
    _astVer[_u32VerNum].u32Pre  = u32Pre;
    _astVer[_u32VerNum].u32Last = u32Last;
    _u32VerNum++;
    return TRUE;
}


SH_BOOL SH_SetVersion(SH_U32 u32Pre,SH_U32 u32Last,SH_S8* ps8Module)
{
    return sh_SetVersion(u32Pre,u32Last,ps8Module);
}

SH_BOOL SH_GetAllVersion(STVer** pstVer,SH_U32* pu32Num)
{
    if ((pstVer == NULL) || (pu32Num == NULL))
    {
        return FALSE;
    }

    *pstVer  = _astVer;
    *pu32Num = _u32VerNum;
    return TRUE;
}


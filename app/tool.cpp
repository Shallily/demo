#include "sh_mathequa.h"
#include "sh_demo.h"
#include <string.h>

static SH_BOOL _st_Calculate(SH_S8* ps8Equa,SH_S8* ps8Param1,SH_S8* ps8Param2,SH_S8* ps8Param3)
{
    static DOUBLE dOut = 0.0;
    EERRCODE eRet;
    SH_U32   u32Out;
    SH_U8    au8Digit[64] = {0};
    SH_U32   u32DigitNum  = 0;
    DOUBLE   adParam[3];

    if (strcmp(ps8Equa,"to2") == 0)
    {
        u32Out = (SH_U32)dOut;
        do
        {
            au8Digit[u32DigitNum++] = u32Out % 2; 
            u32Out = u32Out >> 1;
        }while (u32Out);

        printf("u32Out = ");
        while (u32DigitNum--)
        {
            printf("%d",au8Digit[u32DigitNum]);
        }

        printf("\n");
        return TRUE;
    }
    else if (strcmp(ps8Equa,"to8") == 0)
    {
        printf("u32Out = %o\n",(SH_U32)dOut);
        return TRUE;
    }
    else if (strcmp(ps8Equa,"to10") == 0)
    {
        printf("u32Out = %d\n",(SH_U32)dOut);
        return TRUE;
    }
    else if (strcmp(ps8Equa,"to16") == 0)
    {
        printf("u32Out = 0x%X\n",(SH_U32)dOut);
        return TRUE;
    }

    adParam[0] = SH_DemoMultiuserStrtoNum(ps8Param1);
    adParam[1] = SH_DemoMultiuserStrtoNum(ps8Param2);
    adParam[2] = SH_DemoMultiuserStrtoNum(ps8Param3);
    eRet = SH_MathEqua(ps8Equa,adParam,&dOut);
    if (eRet != E_NOERR)
    {
        return FALSE;
    }

    if (dOut - (SH_U32)dOut == 0.0)
    {
        printf("u32Out = %d\n",(SH_U32)dOut);
    }
    else
    {
        printf("dOut = %lf\n",dOut);
    }
    
    return TRUE;
}

static STDemoFun _astToolDemoFunc[] =
{
    {"cal",0,1,0xf,(SHFunc)_st_Calculate,LSTRING"calculator\nparam1:(SH_S8*)mathequa,such as \"5+8\"\n\
param2:(SH_S8*)parameter\nparam3:(SH_S8*)parameter\nparam4:(SH_S8*)parameter\n\
sample:\ncal 5+8\ncal 2x+1 3\ncal 3x+2y+z 1 2 3\n"},
};

extern "C" void sh_ToolInit(void);

void sh_ToolInit(void)
{
    SH_U32 u32Idx;

    for (u32Idx = 0;u32Idx < SIZEOF(_astToolDemoFunc) / SIZEOF(STDemoFun);u32Idx++)
    {
        SH_DemoMultiuserInsertCmd(_astToolDemoFunc + u32Idx);
    }
}


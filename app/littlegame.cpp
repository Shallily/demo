#include <memory.h>
#include "sh_link.h"
#include "sh_stack.h"
#include "sh_demo.h"

class CLittleGame
{
    public:
        CLittleGame(){SH_LinkInit(&stSteps);};
        virtual ~CLittleGame(){SH_LinkDeInit(&stSteps);};

        SH_S8 Serach();
        
    private:
        STLink stSteps;
        
        virtual SH_U16 GenerateMoves() = 0;
        virtual SH_BOOL MakeMove(SH_U16 u16Move) = 0;
        virtual void UnMakeMove(SH_U16 u16Move) = 0;
        virtual DOUBLE Value(SH_BOOL* pbWin) = 0;
};

SH_S8 CLittleGame::Serach()
{
    SH_BOOL  bWin;
    SH_U16   u16Moves;
    SH_U16   u16Idx;
    SH_S8    s8Ret = 0;
    STNodeL* pstWork;

    Value(&bWin);
    if (bWin)
    {
        pstWork = stSteps.head;
        for (u16Idx = 0;u16Idx < stSteps.u32Num;u16Idx++)
        {
            printf("%d ",*((SH_U16*)(pstWork->pData)));
            pstWork = pstWork->next;
        }

        printf("\n");
        return TRUE;
    }

    u16Moves = GenerateMoves();
    for (u16Idx = 0;u16Idx < u16Moves;u16Idx++)
    {
        if (MakeMove(u16Idx))
        {
            SH_LinkAdd(&stSteps,&u16Idx,SIZEOF(SH_U16));
            s8Ret = Serach();
            SH_LinkDeleteNode(&stSteps,stSteps.head->prev);
            UnMakeMove(u16Idx);
        }
    }

    return s8Ret;
}

class CFarmerRiver:CLittleGame
{
    public:
        CFarmerRiver(){u8State = 0;memset(as8State,0,SIZEOF(as8State));as8State[0] = TRUE;};
        ~CFarmerRiver(){};

    private:
        SH_U8 u8State;
        SH_BOOL as8State[16];
        
        SH_U16 GenerateMoves();
        SH_BOOL MakeMove(SH_U16 u16Move);
        void UnMakeMove(SH_U16 u16Move);
        DOUBLE Value(SH_BOOL* pbWin){*pbWin = (u8State == 0x0f) ? TRUE : FALSE;return 0.0;};
};

SH_U16 CFarmerRiver::GenerateMoves()
{
    //{farmer,farmer+wolf,farmer+sheep,farmer+cabbage}={0 -> 1}
    //{farmer,farmer+wolf,farmer+sheep,farmer+cabbage}={1 -> 0}
    return 8;
}

SH_BOOL CFarmerRiver::MakeMove(SH_U16 u16Move)
{
    SH_U8   u8StateNew;
    SH_BOOL bBack;
    SH_U16  u16Select;

    bBack = u16Move / 4;
    u16Select = u16Move % 4;
    if (bBack)
    {
        if (!(u8State & 0x08) || !(u8State & (0x08 >> u16Select)))
        {
            return FALSE;
        }

        u8StateNew = u8State & (0xf - (0x08 | (0x08 >> u16Select)));
    }
    else
    {
        if ((u8State & 0x08) || (u8State & (0x08 >> u16Select)))
        {
            return FALSE;
        }

        u8StateNew = u8State | 0x08 | (0x08 >> u16Select);
    }
    
    if (as8State[u8StateNew])
    {
        return FALSE;
    }
    
    if ((((u8StateNew & 0x8) >> 3) != ((u8StateNew & 0x2) >> 1))
        && ((((u8StateNew & 0x4) >> 2) == ((u8StateNew & 0x2) >> 1))
        || (((u8StateNew & 0x2) >> 1) == (u8StateNew & 0x1))))
    {
        return FALSE;
    }

    u8State = u8StateNew;
    as8State[u8State] = TRUE;
    return TRUE;
}

void CFarmerRiver::UnMakeMove(SH_U16 u16Move)
{
    SH_BOOL bBack;
    SH_U16  u16Select;

    as8State[u8State] = FALSE;
    bBack = u16Move / 4;
    u16Select = u16Move % 4;
    if (bBack)
    {
        u8State = u8State | 0x08 | (0x08 >> u16Select);
    }
    else
    {
        u8State = u8State & (0xf - (0x08 | (0x08 >> u16Select)));
    }
}

class CMonkRiver:CLittleGame
{
    public:
        CMonkRiver();
        ~CMonkRiver(){SH_StackDeInit(&stState);};

    private:
        SH_U8 u8State;
        SH_BOOL as8State[32];
        STStack stState;
        SH_U8 u8StateCheck;
        
        SH_U16 GenerateMoves();
        SH_BOOL MakeMove(SH_U16 u16Move);
        void UnMakeMove(SH_U16 u16Move);
        DOUBLE Value(SH_BOOL* pbWin){*pbWin = (u8State == 0x7f) ? TRUE : FALSE;return 0.0;};
};

CMonkRiver::CMonkRiver()
{
    u8State = 0;
    memset(as8State,0,SIZEOF(as8State));
    as8State[0] = TRUE;
    SH_StackInit(&stState);
}

SH_U16 CMonkRiver::GenerateMoves()
{
    //{monk,monster,monk+monster,monk+monk,monster+monster}={0 -> 1}
    //{monk,monster,monk+monster,monk+monk,monster+monster}={1 -> 0}
    return 10;
}

#define SELECT(start)   for (u8Idx = start;u8Idx < start + 3;u8Idx++) \
                        { \
                            if (((u8State & (u8Select >> u8Idx)) >> (5 - u8Idx)) \
                                == ((u8State & 0x40) >> 6)) \
                            { \
                                au8Select[u8Idy++] = u8Idx; \
                                if ((u8Idy == u8SelectNum) || (u16Select == 2)) \
                                { \
                                    break; \
                                } \
                            } \
                        }

SH_BOOL CMonkRiver::MakeMove(SH_U16 u16Move)
{
    SH_U8   u8StateNew;
    SH_BOOL bBack;
    SH_U16  u16Select;
    SH_U8   u8Idx;
    SH_U8   u8Select = 0x20;
    SH_U8   u8Idy = 0;
    SH_U8   au8Num[4] = {0};//au8Num[0] monk 0,au8Num[1] monk 1,au8Num[2] monster 0,au8Num[3] monter 1
    SH_U8   u8SelectNum;
    SH_U8   au8Select[2];

    bBack = u16Move / 5;
    u16Select = u16Move % 5;
    u8SelectNum = (u16Select > 1) ? 2 : 1;
    switch (u16Select)
    {
        case 0:
        case 2:
        case 3:
            SELECT(0);
            if ((u16Select == 0) || (u16Select == 3))
            {
                break;
            }
        default:
            SELECT(3);
            break;
    }

    if ((u8Idy < u8SelectNum) || (bBack != ((u8State & 0x40) >> 6)))
    {
        return FALSE;
    }

    if (bBack)
    {
        u8StateNew = (u8State & (~0x40)) & (~(0x20 >> au8Select[0]));
        if (u8SelectNum == 2)
        {
            u8StateNew = u8StateNew & (~(0x20 >> au8Select[1]));
        }
    }
    else
    {
        u8StateNew = (u8State | 0x40) | (0x20 >> au8Select[0]);
        if (u8SelectNum == 2)
        {
            u8StateNew = u8StateNew | (0x20 >> au8Select[1]);
        }
    }

    for (u8Idx = 0;u8Idx < 6;u8Idx++)
    {
        au8Num[u8Idx / 3 * 2 + ((u8State & (u8Select >> u8Idx)) >> (5 - u8Idx))]++;
    }

    u8StateCheck = (((au8Num[0] << 2) | au8Num[2]) << 1) | ((u8StateNew & 0x40) >> 6);
    if ((((au8Num[2] > au8Num[0]) && au8Num[0]) || ((au8Num[3] > au8Num[1]) && au8Num[1]))
        || (as8State[u8StateCheck]))
    {
        return FALSE;
    }

    SH_StackPush(&stState,&u8State,SIZEOF(SH_U8));
    SH_StackPush(&stState,&u8StateCheck,SIZEOF(SH_U8));
    u8State = u8StateNew;
    as8State[u8StateCheck] = TRUE;
    return TRUE;
}

void CMonkRiver::UnMakeMove(SH_U16 u16Move)
{
    SH_StackPop(&stState,&u8StateCheck);
    as8State[u8StateCheck] = FALSE;
    SH_StackPop(&stState,&u8State);
}

SH_BOOL sh_LittleGameTest(void)
{
    CLittleGame* pcGame;
    CFarmerRiver* pcFarmerRiver = new CFarmerRiver();
    CMonkRiver*   pcMonkRiver = new CMonkRiver();

    printf("CFarmerRiver\n");
    pcGame = (CLittleGame*)pcFarmerRiver;
    pcGame->Serach();
    delete pcGame;

    printf("CMonkRiver\n");
    pcGame = (CLittleGame*)pcMonkRiver;
    pcGame->Serach();
    delete pcGame;
    return TRUE;
}

static STDemoFun _astDevDemoFunc[] =
{
    {"river",0,1,0,(SHFunc)sh_LittleGameTest,LSTRING"sh_LittleGameTest\nno param\n"},
};

extern "C" void sh_LittleGameInit(void);

void sh_LittleGameInit(void)
{
    SH_U32 u32Idx;

    for (u32Idx = 0;u32Idx < SIZEOF(_astDevDemoFunc) / SIZEOF(STDemoFun);u32Idx++)
    {
        SH_DemoMultiuserInsertCmd(_astDevDemoFunc + u32Idx);
    }
}


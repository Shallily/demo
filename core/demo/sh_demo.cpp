#include <stdio.h>
#include <memory.h>
#include <map>
#include <string>
#include "sh_demo.h"
#include <unistd.h>

extern void sh_InputStart(void);
extern SH_BOOL sh_InputGetCmd(char* ps8Str);
extern void sh_InputStop(void);
extern void sh_InputInsertCmd(SH_S8* ps8Cmd);

#define SH_MAX_CMD (256)
#define MAX_CMD_PARAMETER (7)

static std::map<std::string,STDemoFun> _stCmdLink;
static SH_BOOL _bCmdInit = FALSE;

static void _sh_GetCmdArgv(SH_S8* ps8LineBuf, SH_U8* pu8argc, SH_S8** argv)
{
    SH_U8  argc;
    SH_U8  argnum;
    SH_S8* ps8Str;

    argc = 0;
    for (argnum = 0; argnum < MAX_CMD_PARAMETER; argnum++)
    {
        argv[argnum] = NULL;
    }
    
    ps8Str = ps8LineBuf;
    argv[argc++] = ps8LineBuf;
    while((ps8Str = strchr(ps8Str,' ')) != NULL)
    {        
        *(ps8Str++) = 0;
        argv[argc++] = ps8Str;
        if (argc >= MAX_CMD_PARAMETER)
        {
            *pu8argc = argc;
            printf("Warring!! :too many parameter\n ");
            return;
        }
    }

    *pu8argc = argc;
}

static SH_BOOL _sh_demoCheckValid(char cChar,SH_BOOL bHex)
{
    if (bHex)
    {
        return ((cChar >= '0') && (cChar <= '9'))
            || ((cChar >= 'a') && (cChar <= 'f'))
            || ((cChar >= 'A') && (cChar <= 'F'));
    }
    else
    {
        return (cChar >= '0') && (cChar <= '9');
    }
}

static DOUBLE _sh_StrtoNum(SH_S8* ps8Str,SH_BOOL* pbDecimal)
{
    DOUBLE  dRonud   = 0.0;
    DOUBLE  dDecimal = 1.0;
    SH_U16  u16Idx   = 0;
    SH_BOOL bHex     = FALSE;

    *pbDecimal = FALSE;
    if (ps8Str == NULL)
    {
        return 0.0;
    }
    
    if (ps8Str[u16Idx] == '-')
    {
        u16Idx++;
    }

    if ((ps8Str[u16Idx] == '0') && (ps8Str[u16Idx + 1] == 'x'))
    {
        u16Idx += 2;
        bHex = TRUE;
    }

    while (_sh_demoCheckValid(ps8Str[u16Idx],bHex))
    {
        if (*pbDecimal == TRUE)
        {            
            dDecimal *= (FLOAT)0.1;
            dRonud = dRonud + dDecimal * (FLOAT)(ps8Str[u16Idx] - '0');
            u16Idx++;
        }
        else
        {
            if ((ps8Str[u16Idx] >= '0') && (ps8Str[u16Idx] <= '9'))
            {
                dRonud = dRonud * (bHex ? 16 : 10) + ps8Str[u16Idx] - '0';
            }
            else if ((ps8Str[u16Idx] >= 'a') && (ps8Str[u16Idx] <= 'f'))
            {
                dRonud = dRonud * 16 + ps8Str[u16Idx] - 'a' + 10;
            }
            else
            {
                dRonud = dRonud * 16 + ps8Str[u16Idx] - 'A' + 10;
            }
            
            u16Idx++;
            if (ps8Str[u16Idx] == '.')
            {
                if (bHex)
                {
                    if (ps8Str[0] == '-')
                    {
                        dRonud = (FLOAT)0.0 - dRonud;
                    }
                    
                    return dRonud;//十六进制数不能为小数，直接返回0.0
                }
                
                *pbDecimal = TRUE;
                u16Idx++;
                continue;
            }            
        }

        if (ps8Str[u16Idx] == '\0')//字符串结束，返回正确数字
        {
            if (ps8Str[0] == '-')
            {
                dRonud = (FLOAT)0.0 - dRonud;
            }
            
            return dRonud;
        }
    }

    if (ps8Str[0] == '-')
    {
        dRonud = (FLOAT)0.0 - dRonud;
    }
    
    return dRonud;//遇到非法字符直接返回0.0
}

static SH_BOOL _sh_RunCmd(SH_S8* ps8Cmd)
{
    SH_BOOL   bknownCmd = FALSE;
    DOUBLE    adInput[6];
    SH_U32    au32Input[6];
    void*     paInput[6];
    SH_S8     *argv[MAX_CMD_PARAMETER + 1];
    SH_U8     argc;
    SH_BOOL   bCmdRet = FALSE;
    SH_U8     u8Idx;
    SH_BOOL   bDecimal;

    _sh_GetCmdArgv(ps8Cmd, &argc, argv);
    if (strcmp(argv[0],"help") == 0)
    {
        if (argc < 2)
        {
            printf("input \"help xxx \" to display help info\n");
            printf("\n<<shallily>> ");
            return TRUE;
        }

        if (_stCmdLink.find(argv[1]) != _stCmdLink.end())
        {
            if (_stCmdLink[argv[1]].ps8Help == NULL)
            {
                printf("no help info\n");
            }
            else
            {
                printf("%s",_stCmdLink[argv[1]].ps8Help);
            }
        }
        else
        {
            if (strcmp(argv[1],"help") == 0)
            {
                printf("display help info\n");
            }
            else if (strcmp(argv[1],"quit") == 0)
            {
                printf("exit system\n");
            }
            else
            {
                printf("no this command\n");
            }
        }

        printf("\n<<shallily>> ");
        return TRUE;
    }

    if (_stCmdLink.find(argv[0]) != _stCmdLink.end())
    {
        for(u8Idx = 0;u8Idx < 6;u8Idx++)
        {
            if (_stCmdLink[argv[0]].u8InputMask & (0x1 << u8Idx))
            {
                paInput[u8Idx] = (argv[u8Idx + 1] != 0) ? argv[u8Idx + 1] : NULL;
            }
            else
            {
                adInput[u8Idx] = _sh_StrtoNum(argv[u8Idx + 1],&bDecimal);
                if (bDecimal == FALSE)
                {
                    au32Input[u8Idx] = (SH_U32)(adInput[u8Idx]);
                    paInput[u8Idx] = &(au32Input[u8Idx]);
                }
                else
                {
                    paInput[u8Idx] = &(adInput[u8Idx]);
                }
            }
        }

        bCmdRet = _stCmdLink[argv[0]].SHFunc(paInput[0],paInput[1],paInput[2],paInput[3],paInput[4],paInput[5]);
        if (bCmdRet)
        {
            printf("<<shallily>> %s, OK", argv[0]);
        }
        else
        {
            printf("<<shallily>> %s, FAIL", argv[0]);
        }

        printf("\n<<shallily>> ");
        bknownCmd = TRUE;
    }
    else
    {
        bknownCmd = FALSE;
    }

    if (argc && !bknownCmd)
    {
        printf("<<shallily>> unknow command");
        printf("\n<<shallily>> ");
    }

    return bCmdRet;
}

void    SH_DemoMultiuserStart(void)
{
    char acCmd[SH_MAX_CMD];

    printf("Press \"Tab\" to display commands\n");
    printf("Input \"help xxx\" to display command's help info\n");
    printf("<<shallily>> ");
    sh_InputStart();
    sh_InputInsertCmd(LSTRING"quit");
    sh_InputInsertCmd(LSTRING"help");
    while (1)
    {
        usleep(100000);
        if (sh_InputGetCmd(acCmd))
        {
            if (strcmp(acCmd,"quit") == 0)
            {
                break;
            }

            _sh_RunCmd(acCmd);
        }
    }

    sh_InputStop();
}

void    SH_DemoMultiuserInsertCmd(STDemoFun* pstDemoFun)
{
    if (pstDemoFun != NULL)
    {
        if (_bCmdInit == FALSE)
        {
            _stCmdLink.clear();
            _bCmdInit = TRUE;
        }
        
        strcpy(_stCmdLink[pstDemoFun->s8CmdName].s8CmdName,pstDemoFun->s8CmdName);
        _stCmdLink[pstDemoFun->s8CmdName].u8InputMask = pstDemoFun->u8InputMask;
        _stCmdLink[pstDemoFun->s8CmdName].SHFunc = pstDemoFun->SHFunc;
        _stCmdLink[pstDemoFun->s8CmdName].ps8Help = pstDemoFun->ps8Help;
        sh_InputInsertCmd(pstDemoFun->s8CmdName);
    }
}

DOUBLE  SH_DemoMultiuserStrtoNum(SH_S8* ps8Str)
{
    SH_BOOL bDecimal;
    
    return _sh_StrtoNum(ps8Str,&bDecimal);
}


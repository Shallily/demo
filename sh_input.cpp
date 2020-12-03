#include "sh_type.h"
#include <string>
#include <list>
#include <termios.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>

using namespace std;

#define MAX_RECORD_CMD                  0x20
#define WAIT_OUT_INPUT                  0x80000000
#define WAIT_INPUT_OVER                 0x40000000

#ifdef ANDROID
#define BACKSPACE                       127
#else
#define BACKSPACE                       8
#endif

#define TABLE                           9
#define ENTER                           10
#define DIRECTION                       91
#define UP                              65
#define DOWN                            66
#define LEFT                            68
#define RIGHT                           67
#define ESC                             27
#define KEYDELETE                       127

std::list<std::string> stCmdQueue;
static SH_BOOL  _bOutInput                        = FALSE;
static SH_S8*   _ps8OutInputBuf                   = NULL;
static SH_BOOL  _bOutInputEnable                  = FALSE;
static std::list<std::string> _stCmdLink;
static SH_BOOL  _bCmdLinkInit                     = FALSE;

static SH_U8    au8CmdStack[MAX_RECORD_CMD][256];
static SH_U8    u8Buttom                          = 0;
static SH_U8    u8Top                             = -1;
static SH_U8    u8Work                            = -1;
static SH_BOOL  bFull                             = FALSE;

static SH_BOOL _bPassWordMode                     = FALSE;
static SH_BOOL _bQuit                             = FALSE;

#define add_work()      {u8Work++;if (u8Work == MAX_RECORD_CMD){if (bFull) {u8Work = 0;}else {u8Work = MAX_RECORD_CMD - 1;}}}
#define dec_work()      {u8Work--;if (u8Work == (SH_U8)-1){if (bFull) {u8Work = MAX_RECORD_CMD - 1;}else {u8Work = 0;}}}
#define get_prev()      if (u8Work + bFull * MAX_RECORD_CMD > u8Buttom){dec_work()}
#define get_next()      if (u8Work < u8Top + bFull * MAX_RECORD_CMD){add_work()}
#define add_buttom()    if (bFull){u8Buttom++;if (u8Buttom == MAX_RECORD_CMD){u8Buttom = 0;}}
#define add_top()       {u8Top++;if (u8Top == MAX_RECORD_CMD){u8Top = 0;bFull = TRUE;} u8Work = u8Top;add_buttom()}

static struct termios oldt;

extern void print_byorderstart(void);
extern void print_byorder(char *str,char attr1);

static void _sh_TerminalRestore(void)
{
    //Apply saved settings
    tcsetattr(0, TCSANOW, &oldt); 
}

//make terminal read 1 char at a time
static void _sh_TerminalSet(void)
{
    struct termios newt;
    
    //save terminal settings
    tcgetattr(0, &oldt); 
    //init new settings
    newt = oldt;  
    //change settings
    newt.c_lflag &= ~(ICANON | ECHO);
    //apply settings
    tcsetattr(0, TCSANOW, &newt);
}

static SH_U8 _sh_GetSameLen(SH_S8* ps8Src,SH_S8* ps8Dst)
{
    SH_U8 u8Len = 0;

    while (*(ps8Src + u8Len) && (*(ps8Src + u8Len) == *(ps8Dst + u8Len)))
    {
        u8Len++;
    }

    return u8Len;
}

static void _sh_GetCmd(char *str)
{
    char       c;
    char       cmd[256];
    SH_U32     work             = 0;
    SH_U32     num              = 0;
    bool       bdirection       = false;
    SH_U32     u32I;
    char       cmdtmp[256];// 用于判断
    char       cLastDirection   = 0;
    std::list<std::string> stCmdQ;
    char       cmdtemp[256];// table 键专用
    char       cmdlast[256];// table 键专用
    SH_U8      u8CmdLastLen;// table 键专用
    std::list<std::string>::iterator iter;
    bool       bPreDirection    = false;

    stCmdQ.clear();
    memset(cmd,'\0',sizeof(cmd));
    while(1)
    {
        c = getchar();
        //printf("c = %d\n",c);

        if (bdirection)
        {
            switch(c)
            {
                case UP:
                case DOWN:
                    if (c == DOWN)
                    {
                        if (cLastDirection == UP)
                        {
                            get_next();
                        }
                        if (u8Work == u8Top)
                        {
                            for (u32I = 1;u32I <= num - work;u32I++)
                            {
                                putchar(' ');
                            }
                            while(num-- )
                            {
                                printf("\b \b");
                            }
                            memset(cmd,'\0',sizeof(cmd));
                            work = num = 0;
                            break;
                        }
                        get_next();
                    }
                    if (u8Work != (SH_U8)-1)
                    {
                        memset(cmd,'\0',sizeof(cmd));
                        strcpy(cmd,(char*)&au8CmdStack[(int)u8Work]);
                        for (u32I = 1;u32I <= num - work;u32I++)
                        {
                            putchar(' ');
                        }
                        while(num-- )
                        {
                            printf("\b \b");
                        }
                        printf("%s",cmd);
                        work = num = strlen(cmd);

                        cLastDirection = c;
                        if ((c == UP) && (u8Work != u8Buttom))
                        {
                            get_prev();
                        }
                        else if (u8Work == u8Buttom)
                        {
                            cLastDirection = 0;
                        }
                    }

                    break;
                case LEFT:
                    if (work)
                    {
                        printf("\b");
                        work--;
                    }
                    break;
                case RIGHT:
                    if (work < num)
                    {
                        printf("%c",cmd[work]);
                        work++;
                    }
                    break;
                default:
                    break;
            }
            bdirection     = false;
            continue;
        }

        if ((c != DIRECTION) && (c != ESC))
        {
            u8Work = u8Top;
            cLastDirection = 0;
        }
        
        if (c == BACKSPACE)
        {
            if (work && num)
            {
                work--;
                num--;
                if (work < num)
                {
                    for (u32I = 1;u32I <= num - work + 1;u32I++)
                    {
                        cmd[work + u32I - 1]    = cmd[work + u32I];
                        cmdtmp[work + u32I - 1] = cmdtmp[work + u32I];
                    }
                    printf("\b%s \b",cmd + work);
                    for (u32I = 0;u32I < num - work;u32I++)
                    {
                        printf("\b");
                    }
                }
                else
                {
                    printf("\b \b");
                    cmd[num] = '\0';
                    cmdtmp[num] = '\0';
                }
            }
        }
        else if (c == ESC)
        {
            _bQuit++;
            bPreDirection = true;
        }
        else if (c == DIRECTION)
        {
            if (bPreDirection == true)
            {
                bdirection    = true;
                bPreDirection = false;
                _bQuit--;
            }
        }
        else if (c == ENTER)
        {
            if (num)
            {
                printf("\n");
                cmd[num] = '\0';
                if (_bOutInput)
                {
                    if (_ps8OutInputBuf != NULL)
                    {
                        strcpy(_ps8OutInputBuf,cmd);
                    }
                    else
                    {
                        if (strcmp(cmd,"ok") == 0)
                        {
                            _bOutInputEnable = TRUE;
                        }
                        else if (strcmp(cmd,"no") == 0)
                        {
                            _bOutInputEnable = FALSE;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    memset(cmd,'\0',sizeof(cmd));
                    memset(cmdtmp,'\0',sizeof(cmdtmp));
                    num = work = 0;
                    continue;
                }
                
                break;
            }
            
            continue;
        }
        else if (c == TABLE)
        {
            cmdtmp[work] = '\0';
            iter = _stCmdLink.begin();
            for (iter = _stCmdLink.begin(); iter != _stCmdLink.end(); iter++)
            {
                if (strncmp((char *)cmd, iter->c_str(),num) == 0)
                {
                    stCmdQ.push_back(iter->c_str());
                }
            }
            if (stCmdQ.size() == 1)
            {
                while(work--)
                {
                    printf("\b \b");
                }
                
                memset(cmd,'\0',sizeof(cmd));
                strcpy(cmd,stCmdQ.front().c_str());
                stCmdQ.pop_front();
                strcpy(cmdtmp,cmd);
                printf("%s",cmd);
                work = num = strlen((char*)cmd);
            }
            else if (stCmdQ.size() != 0)
            {
                cmdlast[0]   = '\0';
                u8CmdLastLen = 0;
                printf("\n");
                print_byorderstart();
                while(stCmdQ.size())
                {
                    memset(cmdtemp,'\0',sizeof(cmdtemp));
                    strcpy(cmdtemp,stCmdQ.front().c_str());
                    stCmdQ.pop_front();
                    if (u8CmdLastLen == 0)
                    {
                        strcpy(cmdlast,cmdtemp);
                        u8CmdLastLen = strlen(cmdlast);
                    }
                    else
                    {
                        u8CmdLastLen = _sh_GetSameLen(cmdlast,cmdtemp);
                        cmdlast[u8CmdLastLen] = '\0';
                    }
                    
                    //printf("%s\n",cmdtemp);
                    print_byorder(cmdtemp,0);
                }

                printf("\n");
                if (work)
                {
                    strcpy(cmd,cmdlast);
                    printf("%s",cmd);
                    work = num = u8CmdLastLen;
                }
            }

            stCmdQ.clear();
        }
        else if (c == KEYDELETE)
        {
            if (work < num - 1)
            {
                num--;
                for (u32I = 2;u32I <= num - work + 1;u32I++)
                {
                    cmd[work + u32I - 1]    = cmd[work + u32I];
                    cmdtmp[work + u32I - 1] = cmdtmp[work + u32I];
                }
                printf("%s \b",cmd + work);
                for (u32I = 0;u32I < num - work;u32I++)
                {
                    printf("\b");
                }
            }
        }
        else
        {
            if (work < num)
            {
                for (u32I = num - work + 1;u32I > 0;u32I--)
                {
                    cmd[work + u32I]    = cmd[work + u32I - 1];
                    cmdtmp[work + u32I] = cmdtmp[work + u32I - 1];
                }
                printf("%c%s",c,cmd + work + 1);
                for (u32I = 0;u32I < num - work;u32I++)
                {
                    printf("\b");
                }
            }
            else
            {
                if (_bPassWordMode == TRUE)
                {
                    printf("*");
                }
                else
                {
                    printf("%c",c);
                }
            }
            cmdtmp[work] = c;
            cmd[work++] = c;
            num++;
        }
    }

    strcpy(str,cmd);
    stCmdQueue.push_back(cmd);
    if (_bPassWordMode == FALSE)
    {
        add_top();
        //printf("%d %d %d\n",u8Buttom,u8Top,u8Work);
        strcpy((char*)&au8CmdStack[(int)u8Work],cmd);
    }
    else
    {
        strcpy(str,"quit");
    }
}

static void _sh_InputTask(void* pAddr)
{
    char acStr[256];
    
    do
    {
        _sh_GetCmd(acStr);
    }while(strcmp(acStr,"quit"));
}

static void _sh_CreateThread(pthread_t *thread, void *routine, void *param)
{
    int retval = 0;
	pthread_attr_t attr;
    
    pthread_attr_init(&attr);
	retval = pthread_create(thread, &attr, (void *(*)(void *)) routine, (void *) param);
    if(retval != 0)
    {
        printf("create thread failed(%s)!\n", strerror(retval));
    }
    
    pthread_attr_destroy(&attr);
}

void    sh_InputStart(void)
{
    pthread_t thread;
    
    stCmdQueue.clear();
    if (_bCmdLinkInit == FALSE)
    {
        _stCmdLink.clear();
        _bCmdLinkInit = TRUE;
    }

    #if (SH_OS_TYPE == SH_LINUX)
    _sh_TerminalSet();
    #endif
    _sh_CreateThread(&thread, (void *)_sh_InputTask, NULL);
}

void    sh_InputStop(void)
{
    stCmdQueue.clear();
    _stCmdLink.clear();
    #if (SH_OS_TYPE == SH_LINUX)
    _sh_TerminalRestore();
    #endif
}

SH_BOOL sh_InputGetCmd(SH_S8* ps8Str)
{
    if (!stCmdQueue.empty())
    {
        strcpy(ps8Str,stCmdQueue.front().c_str());
        stCmdQueue.pop_front();
        return TRUE;
    }

    return FALSE;
}

void    sh_InputInsertCmd(SH_S8* ps8Cmd)
{
    if (_bCmdLinkInit == FALSE)
    {
        _stCmdLink.clear();
        _bCmdLinkInit = TRUE;
    }
    
    _stCmdLink.push_back(ps8Cmd);
}


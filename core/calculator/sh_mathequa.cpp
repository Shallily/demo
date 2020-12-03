//2017/06/14 V0.1
//2017/06/16 V0.2
//优化算法
//2017/06/19 V0.3
//1.优化“+”“-”“*”“/”的合法性判断
//2.修改连续“^”运算异常问题
//2017/06/21 V0.4
//增加数学函数合法性判断
//2017/07/26 V0.5
//修改POW 运算时合法性判断条件不对问题
//2018/01/12 V0.6
//增加多参数支持，最多可以支持3 个参数，分别用x 、y、z
//2018/01/27 V0.7
//1.修改乘除法适应空格错误问题
//2.修改((x+1)^(6)) 这种形式计算错误问题
//2018/07/18 V0.8
//增加16进制支持
#ifndef _SH_MATHEQUA_DLL_
#define _SH_MATHEQUA_DLL_
#endif

#include "sh_mathequa.h"
#include "sh_version.h"
#include "sh_stack.h"
#include <math.h>
#include "sh_version.h"

#define NUM  1
#define OPR  2

#define GET_NUM() dTmpRonud   = 0.0; \
        dTmpDecimal = 1.0; \
        bDecimal    = FALSE; \
        bHasNum     = FALSE;bHex = FALSE; \
        if ((ps8Equa[u16Idx] == '0') && (ps8Equa[u16Idx + 1] == 'x')) \
        {u16Idx += 2;bHex = TRUE;} \
        while (_sh_MathCheckValid(ps8Equa[u16Idx],bHex)) \
        { \
            if (bDecimal == TRUE) \
            { \
                dTmpDecimal *= 0.1; \
                dTmpRonud = dTmpRonud + dTmpDecimal * (ps8Equa[u16Idx] - '0'); \
                u16Idx++; \
            } \
            else \
            { \
                if ((ps8Equa[u16Idx] >= '0') && (ps8Equa[u16Idx] <= '9')) \
                {\
                    dTmpRonud = dTmpRonud * (bHex ? 16 : 10) + ps8Equa[u16Idx] - '0'; \
                }\
                else if ((ps8Equa[u16Idx] >= 'a') && (ps8Equa[u16Idx] <= 'f')) \
                {\
                    dTmpRonud = dTmpRonud * 16 + ps8Equa[u16Idx] - 'a' + 10; \
                }\
                else \
                {\
                    dTmpRonud = dTmpRonud * 16 + ps8Equa[u16Idx] - 'A' + 10; \
                }\
                u16Idx++; \
                if (ps8Equa[u16Idx] == '.') \
                { \
                    if (bHex) \
                    { \
                        SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx])); \
                        SH_StackDeInit(&stEquaS); \
                        return E_PARAM; \
                    } \
                    bDecimal = TRUE; \
                    u16Idx++; \
                } \
            } \
        } \
        bHasNum = TRUE

#define MATHPUSH(opr) if (bHasNum == TRUE) \
                { \
                    stOpr.u8Type = NUM; \
                    stOpr.dNum   = dTmpRonud; \
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr)); \
                    stOpr.u8Type = OPR; \
                    stOpr.enOpr  = E_MUL; \
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr)); \
                } \
                stOpr.u8Type = OPR; \
                stOpr.enOpr  = opr; \
                SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr)); \
                stOpr.u8Type = OPR; \
                stOpr.enOpr  = E_LEFT; \
                SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr)); \
                bHasNum = FALSE;

typedef enum
{
    E_PLUS,
    E_MINUS,
    E_MUL,
    E_DIV,
    E_LEFT,
    E_RIGHT,
    E_SIN,
    E_COS,
    E_TAN,
    E_ARCSIN,
    E_ARCCOS,
    E_ARCTAN,
    E_POW,
    E_SQRT,
    E_LOG,
    E_LOG10,
    E_MAX
}ENOpr;

typedef struct
{
    SH_U8  u8Type;
    DOUBLE dNum;
    ENOpr  enOpr;
}STOpr;

#define SH_DEBUG_LEVEL      _u16MathDbgLvl

#if SH_DEBUG
static SH_U16  _u16MathDbgLvl = SH_DEBUGLEVEL_ERR;
#endif

static SH_BOOL _sh_MathCheckValid(char cChar,SH_BOOL bHex)
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

EERRCODE SH_MathEqua(SH_S8* ps8Equa,DOUBLE* pdInput,DOUBLE* pdOut)
{
    SH_U16  u16Idx = 0;
    STOpr   stOpr;
    STStack stEquaS;
    DOUBLE  dTmpRonud;
    DOUBLE  dTmpDecimal;
    SH_BOOL bDecimal;
    SH_BOOL bHasNum = FALSE;
    DOUBLE  dTmp;
    ENOpr   enOpr;
    SH_S32  s32Tmp;
    SH_U8   u8Idx;
    DOUBLE  adIn[3];
    SH_S8*  ps8Str;
    SH_U8   u8Status = 0;
    SH_BOOL bEmpty;
    SH_BOOL bHex;

    CHECKPOINTER(ps8Equa);
    CHECKPOINTER(pdOut);
    SH_StackInit(&stEquaS);
    for (u8Idx = 0;u8Idx < 3;u8Idx++)
    {
        ps8Str = strchr(ps8Equa,'x' + u8Idx);
        if (ps8Str != NULL)
        {
            adIn[u8Idx] = pdInput[u8Status];
            u8Status++;
        }
    }
    
    while(ps8Equa[u16Idx])
    {
        //"+" "-"前面可以有数，也可以没数，没数时用0 补充；
        //后面不能为"+" "-" "*" "/" "^"
        if ((ps8Equa[u16Idx] == '+') || (ps8Equa[u16Idx] == '-'))
        {
            u16Idx++;
            if((ps8Equa[u16Idx] == '+') || (ps8Equa[u16Idx] == '-')
                || (ps8Equa[u16Idx] == '*') || (ps8Equa[u16Idx] == '/')
                || (ps8Equa[u16Idx] == '^') || (ps8Equa[u16Idx] == '\0'))
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
            
            if (bHasNum == TRUE)
            {
                stOpr.dNum = dTmpRonud;
            }
            else
            {
                stOpr.dNum = 0;
            }
            
            stOpr.u8Type = NUM;
            SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            stOpr.u8Type = OPR;
            if (ps8Equa[u16Idx - 1] == '+')
            {
                stOpr.enOpr  = E_PLUS;
            }
            else
            {
                stOpr.enOpr  = E_MINUS;
            }
            
            SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            bHasNum = FALSE;
        }
        //"*" "/" 前面必须有数；后面不能为"+" "-" "*" "/" "^"
        else if ((ps8Equa[u16Idx] == '*') || (ps8Equa[u16Idx] == '/'))
        {
            u16Idx++;
            if((ps8Equa[u16Idx] == '+') || (ps8Equa[u16Idx] == '-')
                || (ps8Equa[u16Idx] == '*') || (ps8Equa[u16Idx] == '/')
                || (ps8Equa[u16Idx] == '^') || (ps8Equa[u16Idx] == '\0'))
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }

            if (ps8Equa[u16Idx - 1] == '*')
            {
                enOpr = E_MUL;
            }
            else
            {
                enOpr = E_DIV;
            }
            
            if (bHasNum == TRUE)
            {
                while (ps8Equa[u16Idx] == ' ')
                {
                    u16Idx++;
                }
                
                //"*" "/" 后面如果是数并且数后面不是"^" 就立即计算
                if (((ps8Equa[u16Idx] >= '0') && (ps8Equa[u16Idx] <= '9'))
                    || (ps8Equa[u16Idx] == 'x') || (ps8Equa[u16Idx] == 'y')
                    || (ps8Equa[u16Idx] == 'z'))
                {
                    dTmp = dTmpRonud;
                    if (ps8Equa[u16Idx] == 'x')
                    {
                        u16Idx++;
                        dTmpRonud = adIn[0];
                    }
                    else if (ps8Equa[u16Idx] == 'y')
                    {
                        u16Idx++;
                        dTmpRonud = adIn[1];
                    }
                    else if (ps8Equa[u16Idx] == 'z')
                    {
                        u16Idx++;
                        dTmpRonud = adIn[2];
                    }
                    else
                    {
                        GET_NUM();
                    }
                    
                    if (ps8Equa[u16Idx] == '^')
                    {
                        stOpr.u8Type = NUM;
                        stOpr.dNum   = dTmp;
                        SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                        stOpr.u8Type = OPR;
                        stOpr.enOpr  = enOpr;
                        SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                        continue;
                    }

                    if (enOpr == E_MUL)
                    {
                        dTmp *= dTmpRonud;
                    }
                    else
                    {
                        if (dTmpRonud == 0)
                        {
                            SH_DEBUG_ERR(("div num cannot be 0 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }
                        
                        dTmp /= dTmpRonud;
                    }
                    
                    dTmpRonud = dTmp;
                    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                }
                else
                {
                    stOpr.u8Type = NUM;
                    stOpr.dNum   = dTmpRonud;
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                    stOpr.u8Type = OPR;
                    stOpr.enOpr  = enOpr;
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                    bHasNum = FALSE;
                }
            }
            else
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
        }      
        else if (ps8Equa[u16Idx] == '(')
        {
            u16Idx++;
            if (bHasNum == TRUE)
            {
                //数据直接遇到括号，当乘法处理
                stOpr.u8Type = NUM;
                stOpr.dNum   = dTmpRonud;
                SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                stOpr.u8Type = OPR;
                stOpr.enOpr  = E_MUL;
                SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            }
            
            stOpr.u8Type = OPR;
            stOpr.enOpr  = E_LEFT;
            SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            bHasNum = FALSE;
        }
        //遇右括号消除左括号
        else if (ps8Equa[u16Idx] == ')')
        {
            u16Idx++;
            if (SH_StackIsEmpty(&stEquaS) == TRUE)
            {
                SH_DEBUG_ERR(("there is ) but no ( %d,cur result is %f\n",__LINE__,dTmpRonud));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }

            dTmp = dTmpRonud;
            dTmpRonud = 0;
            while(SH_StackIsEmpty(&stEquaS) == FALSE)
            {
                SH_StackPop(&stEquaS,&stOpr);
                //最内层括号里面只有加减乘除运算
                if (stOpr.u8Type == NUM)
                {
                    dTmp = stOpr.dNum;
                    if (SH_StackIsEmpty(&stEquaS) == TRUE)
                    {
                        SH_DEBUG_ERR(("there is ) but no ( %d,cur result is %f\n",__LINE__,dTmpRonud));
                        SH_StackDeInit(&stEquaS);
                        return E_PARAM;
                    }
                    
                    continue;
                }
                else if (stOpr.enOpr == E_PLUS)
                {
                    enOpr = stOpr.enOpr;
                    dTmpRonud += dTmp;
                    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                    continue;
                }
                else if (stOpr.enOpr == E_MINUS)
                {
                    enOpr = stOpr.enOpr;
                    dTmpRonud -= dTmp;
                    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                    continue;
                }
                else if (stOpr.enOpr == E_MUL)
                {
                    SH_StackPop(&stEquaS,&stOpr);
                    dTmpRonud *= stOpr.dNum;
                    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                    continue;
                }
                else if (stOpr.enOpr == E_DIV)
                {
                    SH_StackPop(&stEquaS,&stOpr);
                    if (dTmpRonud == 0)
                    {
                        SH_DEBUG_ERR(("div num cannot be 0 %d\n",__LINE__));
                        SH_StackDeInit(&stEquaS);
                        *pdOut = (DOUBLE)0x7fffffffffffffff;
                        return E_PARAM;
                    }
                    
                    dTmpRonud = stOpr.dNum / dTmpRonud;
                    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                    continue;
                }
                else if (stOpr.enOpr == E_LEFT)
                {
                    dTmpRonud += dTmp;
                    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                    if (SH_StackIsEmpty(&stEquaS) == TRUE)
                    {
                        continue;
                    }
                }
                else
                {
                    SH_DEBUG_ERR(("cannot go here %d\n",__LINE__));
                    SH_StackDeInit(&stEquaS);
                    return E_OTHER;
                }

                //消除一层括号后，需要根据优先级顺序依次判断
                //1.优先判断括号后面是否为"^"
                SH_StackPop(&stEquaS,&stOpr);
                if (ps8Equa[u16Idx] == '^')
                {
                    if ((stOpr.enOpr == E_SIN) || (stOpr.enOpr == E_COS)
                        || (stOpr.enOpr == E_TAN) || (stOpr.enOpr == E_ARCSIN)
                        || (stOpr.enOpr == E_ARCCOS) || (stOpr.enOpr == E_ARCTAN)
                        || (stOpr.enOpr == E_SQRT) || (stOpr.enOpr == E_LOG)
                        || (stOpr.enOpr == E_LOG10))
                    {
                        SH_DEBUG_ERR(("input error %d %d %d\n",__LINE__,stOpr.u8Type,stOpr.enOpr));
                        SH_StackDeInit(&stEquaS);
                        return E_PARAM;
                    }
                    
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                    break;
                }

                //2.判断括号前面是否为数学函数运算或者"^" 运算
                switch(stOpr.enOpr)
                {
                    case E_SIN:
                        dTmpRonud = sin(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_COS:
                        dTmpRonud = cos(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_TAN:
                        dTmpRonud = tan(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_ARCSIN:
                        if (dTmpRonud > 1.0)
                        {
                            SH_DEBUG_ERR(("sin(x) cannot be above 1 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }
                        
                        dTmpRonud = asin(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_ARCCOS:
                        if (dTmpRonud > 1.0)
                        {
                            SH_DEBUG_ERR(("cos(x) cannot be above 1 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }
                        
                        dTmpRonud = acos(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_ARCTAN:
                        dTmpRonud = atan(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_POW:
                        while(stOpr.enOpr == E_POW)
                        {
                            SH_StackPop(&stEquaS,&stOpr);
                            s32Tmp = (SH_S32)(1.0 / dTmpRonud / 2.0);
                            if ((1.0 / dTmpRonud / 2.0 - s32Tmp == 0.0)
                                && (stOpr.dNum < 0))
                            {
                                SH_DEBUG_ERR(("x^(2n) cannot be below 0(%f) %d\n",stOpr.dNum,__LINE__));
                                SH_StackDeInit(&stEquaS);
                                *pdOut = (DOUBLE)0x7fffffffffffffff;
                                return E_PARAM;
                            }
                        
                            dTmpRonud = pow(stOpr.dNum,dTmpRonud);
                            SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                            if (SH_StackIsEmpty(&stEquaS) == TRUE)
                            {
                                bEmpty = TRUE;
                                break;
                            }

                            bEmpty = FALSE;
                            SH_StackPop(&stEquaS,&stOpr);
                        }

                        if (bEmpty == TRUE)
                        {
                            continue;
                        }
                        
                        break;
                    case E_SQRT:
                        if (dTmpRonud < 0.0)
                        {
                            SH_DEBUG_ERR(("x^(2) cannot be below 0 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }
                        
                        dTmpRonud = sqrt(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_LOG:
                        if (dTmpRonud < 0.0)
                        {
                            SH_DEBUG_ERR(("a^(x) cannot be below 0 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }
                        
                        dTmpRonud = log(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    case E_LOG10:
                        if (dTmpRonud < 0.0)
                        {
                            SH_DEBUG_ERR(("a^(x) cannot be below 0 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }
                        
                        dTmpRonud = log10(dTmpRonud);
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            continue;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        break;
                    default:
                        break;
                }

                //3.判断前面是否为乘除运算
                switch(stOpr.enOpr)
                {
                    case E_MUL:
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            break;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        dTmpRonud *= stOpr.dNum;
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        break;
                    case E_DIV:
                        if (SH_StackIsEmpty(&stEquaS) == TRUE)
                        {
                            break;
                        }
                        
                        SH_StackPop(&stEquaS,&stOpr);
                        if (dTmpRonud == 0)
                        {
                            SH_DEBUG_ERR(("div num cannot be 0 %d\n",__LINE__));
                            SH_StackDeInit(&stEquaS);
                            *pdOut = (DOUBLE)0x7fffffffffffffff;
                            return E_PARAM;
                        }

                        dTmpRonud = stOpr.dNum / dTmpRonud;
                        SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
                        break;
                    default:
                        //加减运算或者括号需要还原回去
                        SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                        break;
                }

                //4.判断完毕，暂停括号内运算，继续处理后面的字符
                break;
            }
        }
        //"x"前面有数且后面不是"^" 直接相乘，前面没数直接赋值；
        //后面可以是任意符号
        else if ((ps8Equa[u16Idx] == 'x') || (ps8Equa[u16Idx] == 'y')
            || (ps8Equa[u16Idx] == 'z'))
        {
            u8Idx = ps8Equa[u16Idx] - 'x';
            u16Idx++;
            if (bHasNum == TRUE)
            {
                if (ps8Equa[u16Idx] == '^')
                {
                    stOpr.u8Type = NUM;
                    stOpr.dNum   = dTmpRonud;
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                    stOpr.u8Type = OPR;
                    stOpr.enOpr  = E_MUL;
                    SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                    dTmpRonud = adIn[u8Idx];
                    continue;
                }
                
                dTmpRonud *= adIn[u8Idx];
                SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
            }
            else
            {
                dTmpRonud = adIn[u8Idx];
                bHasNum   = TRUE;
            }
        }
        //数学函数运算以及"^" 后面必须紧跟括号
        else if (ps8Equa[u16Idx] == 's')
        {
            if ((ps8Equa[u16Idx + 1] == 'i') && (ps8Equa[u16Idx + 2] == 'n')
                && (ps8Equa[u16Idx + 3] == '('))
            {
                u16Idx += 4;
                MATHPUSH(E_SIN);
            }
            else if ((ps8Equa[u16Idx + 1] == 'q') && (ps8Equa[u16Idx + 2] == 'r')
                && (ps8Equa[u16Idx + 3] == 't') && (ps8Equa[u16Idx + 4] == '('))
            {
                u16Idx += 5;
                MATHPUSH(E_SQRT);
            }
            else
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
            
        }
        else if (ps8Equa[u16Idx] == 'c')
        {
            if ((ps8Equa[u16Idx + 1] != 'o') || (ps8Equa[u16Idx + 2] != 's')
                || (ps8Equa[u16Idx + 3] != '('))
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
            
            u16Idx += 4;
            MATHPUSH(E_COS);
        }
        else if (ps8Equa[u16Idx] == 't')
        {
            if ((ps8Equa[u16Idx + 1] != 'a') || (ps8Equa[u16Idx + 2] != 'n')
                || (ps8Equa[u16Idx + 3] != '('))
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
            
            u16Idx += 4;
            MATHPUSH(E_TAN);
        }
        else if (ps8Equa[u16Idx] == 'a')
        {
            if ((ps8Equa[u16Idx + 1] == 's') && (ps8Equa[u16Idx + 2] == 'i')
                && (ps8Equa[u16Idx + 3] == 'n') && (ps8Equa[u16Idx + 4] == '('))
            {
                u16Idx += 5;
                MATHPUSH(E_ARCSIN);
            }
            else if ((ps8Equa[u16Idx + 1] == 'c') && (ps8Equa[u16Idx + 2] == 'o')
                && (ps8Equa[u16Idx + 3] == 's') && (ps8Equa[u16Idx + 4] == '('))
            {
                u16Idx += 5;
                MATHPUSH(E_ARCCOS);
            }
            else if ((ps8Equa[u16Idx + 1] == 't') && (ps8Equa[u16Idx + 2] == 'a')
                && (ps8Equa[u16Idx + 3] == 'n') && (ps8Equa[u16Idx + 4] == '('))
            {
                u16Idx += 5;
                MATHPUSH(E_ARCTAN);
            }
            else
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
        }
        else if (ps8Equa[u16Idx] == 'l')
        {
            if ((ps8Equa[u16Idx + 1] != 'o') || (ps8Equa[u16Idx + 2] != 'g'))
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }

            if ((ps8Equa[u16Idx + 3] == '1') && (ps8Equa[u16Idx + 4] == '0')
                && (ps8Equa[u16Idx + 5] == '('))
            {
                u16Idx += 6;
                MATHPUSH(E_LOG10);
            }
            else if (ps8Equa[u16Idx + 3] == '(')
            {
                u16Idx += 4;
                MATHPUSH(E_LOG);
            }
            else
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }
        }
        else if (ps8Equa[u16Idx] == '^')
        {
            //"^"前面必须有数
            if ((ps8Equa[u16Idx + 1] != '('))
            {
                SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
                SH_StackDeInit(&stEquaS);
                return E_PARAM;
            }

            u16Idx += 2;
            stOpr.u8Type = NUM;
            if (bHasNum == TRUE)
            {
                stOpr.dNum   = dTmpRonud;
            }
            else
            {
                stOpr.dNum   = 1.0;
            }

            SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            stOpr.u8Type = OPR;
            stOpr.enOpr  = E_POW;
            SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            stOpr.u8Type = OPR;
            stOpr.enOpr  = E_LEFT;
            SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
            bHasNum = FALSE;
        }
        //普通数字要么是第一个字符，要么前面一定有数据或者左括号
        else if ((ps8Equa[u16Idx] >= '0') && (ps8Equa[u16Idx] <= '9'))
        {
            if (bHasNum == TRUE)
            {
                dTmp = dTmpRonud;
            }
            else
            {
                dTmp = 1.0;
            }
            
            GET_NUM();
            if (ps8Equa[u16Idx] == '^')
            {
                stOpr.u8Type = NUM;
                stOpr.dNum   = dTmp;
                SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                stOpr.u8Type = OPR;
                stOpr.enOpr  = E_MUL;
                SH_StackPush(&stEquaS,&stOpr,sizeof(STOpr));
                continue;
            }
            
            dTmpRonud *= dTmp;
            SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
        }
        else if (ps8Equa[u16Idx] == ' ')
        {
            u16Idx++;
        }
        else
        {
            SH_DEBUG_ERR(("input error %d %d %c\n",__LINE__,u16Idx,ps8Equa[u16Idx]));
            SH_StackDeInit(&stEquaS);
            return E_PARAM;
        }
    }

    dTmp = dTmpRonud;
    dTmpRonud = 0;
    while(SH_StackIsEmpty(&stEquaS) == FALSE)
    {
        SH_StackPop(&stEquaS,&stOpr);
        if (stOpr.u8Type == NUM)
        {
            dTmp = stOpr.dNum;
        }
        else if (stOpr.enOpr == E_PLUS)
        {
            enOpr = stOpr.enOpr;
            dTmpRonud += dTmp;
            SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
        }
        else if (stOpr.enOpr == E_MINUS)
        {
            enOpr = stOpr.enOpr;
            dTmpRonud -= dTmp;
            SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
        }
        else if (stOpr.enOpr == E_LEFT)
        {
            SH_DEBUG_ERR(("there is ( but no ) %d,cur result is %f\n",__LINE__,dTmpRonud));
            SH_StackDeInit(&stEquaS);
            return E_PARAM;
        }
        else
        {
            SH_DEBUG_ERR(("cannot go here %d %d %d\n",__LINE__,stOpr.u8Type,stOpr.enOpr));
            SH_StackDeInit(&stEquaS);
            return E_OTHER;
        }
    }
    
    dTmpRonud += dTmp;
    SH_DEBUG_INFO(("[shallily]%d %f\n",__LINE__,dTmpRonud));
    SH_StackDeInit(&stEquaS);
    *pdOut = dTmpRonud;
    return E_NOERR;
}

VOID     SH_MathEquaVersion(void)
{
    SH_SetVersion(0,8,LSTRING"mathequa");
}


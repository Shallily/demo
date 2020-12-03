//2017/06/16 V0.2
//增加宏CHECKPOINTER和ANGLE
//2017/10/23 V0.3
//增加宏ABS
//2018/02/05 V0.4
//1.增加宏CHECKPT 和CHECKPTEX
//2.增加check 失败时的错误打印
#ifndef _SH_COMMON_H_
#define _SH_COMMON_H_

#include "sh_config.h"
#include "sh_type.h"

#define MAX_ROOTPATH        (0x20)
#define MAX_FILE_NAME       (256)
#define SH_MAX_CMD          (256)
#define BUF_SIZE            (1024 * 1024)
#define MAX_DATE_LEN        (32)
#define MAX_FILES           (100)
#define MAX_FILESIZE        (0x10000)

#define SH_EMPTY(p,type)    {if (p == NULL) {SH_DEBUG_ERR(("check error\n"));return (type)FALSE;}}
#define CHECKPOINTER(x)     {if (x == NULL) {SH_DEBUG_ERR(("check error\n"));return E_NULL;}}
#define CHECKPT(x)          {if (x == NULL) {SH_DEBUG_ERR(("check error\n"));return;}}
#define CHECKPTEX(x,y)      {if (x == NULL) {SH_DEBUG_ERR(("check error\n"));return y;}}

#define PI                  (3.1415926535)
#define RADIAN(angle)       ((angle) * PI / 180.0) //convert angle to radian
#define ANGLE(radian)       ((radian) * 180.0 / PI)
#define MAX(a,b)            (a > b ? a : b)
#define MIN(a,b)            (a < b ? a : b)
#define ABS(Num)            (((Num) > 0.0) ? (Num) : (-(Num)))

#define SHMAKEWORD(hi,lo)     (hi << 8 | lo)
#define SHMAKEDWORD(hi,lo)    (hi << 16 | lo)
#define SHMAKEDWORD1(a,b,c,d) (a << 24 | b << 16 | c << 8 | d)

#define SIZEOF(x)           ((SH_U32)(sizeof(x)))

#endif


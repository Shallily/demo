//2015/10/22 V0.1
//2016/07/05 V0.2
//增加宏SH_DEBUG_PRT(x)
//2019/01/29 V0.3
//调整DEBUG信息
//2019/04/12 V0.4
//优化跨平台编译配置
#ifndef _SH_CONFIG_H_
#define _SH_CONFIG_H_

#define SH_WINDOWS            0
#define SH_LINUX              1

#ifdef LINUX
#define SH_OS_TYPE            SH_LINUX
#else
#define SH_OS_TYPE            SH_WINDOWS
#endif

#if(SH_OS_TYPE == SH_LINUX)
#define LSTRING               (SH_S8*)
#else
#define LSTRING
#endif

#define SH_DEBUG              1

#if SH_DEBUG
#include <stdio.h>

#define SH_DEBUGLEVEL_ERR     0x00000001
#define SH_DEBUGLEVEL_WRN     0x00000002
#define SH_DEBUGLEVEL_INFO    0x00000004
#define SH_DEBUGLEVEL_FUNC    0x00000008

#define SH_DEBUG_FILE_LINE()  printf("%s %d ",__FILE__,__LINE__)
#define SH_DEBUG_ERR(x)       if (SH_DEBUG_LEVEL & SH_DEBUGLEVEL_ERR) {SH_DEBUG_FILE_LINE();printf("[error]");printf x;}
#define SH_DEBUG_WRN(x)       if (SH_DEBUG_LEVEL & SH_DEBUGLEVEL_WRN) {SH_DEBUG_FILE_LINE();printf("[warning]");printf x;}
#define SH_DEBUG_INFO(x)      if (SH_DEBUG_LEVEL & SH_DEBUGLEVEL_INFO) {SH_DEBUG_FILE_LINE();printf("[info]");printf x;}
#define SH_DEBUG_FUNC()       if (SH_DEBUG_LEVEL & SH_DEBUGLEVEL_FUNC) {SH_DEBUG_FILE_LINE();printf("[func]");}
#define SH_DEBUG_PRT(x)       printf x
#else
#define SH_DEBUG_ERR(x)
#define SH_DEBUG_WRN(x)
#define SH_DEBUG_INFO(x)
#define SH_DEBUG_FUNC()
#define SH_DEBUG_PRT(x)
#endif

#endif
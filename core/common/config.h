//2015/10/24
#define sh_enable          1
#define sh_disable         0

#ifdef _DLL_

#define SH_FS              sh_enable
#define SH_OS              sh_enable
#define SH_SOCKET          sh_enable
#define SH_LINK            sh_enable
#define SH_QUEUE           sh_enable
#define SH_STACK           sh_enable

#endif

#ifdef _TCPSRC_

#define SH_CHAT            sh_enable

#endif

#ifdef LINUX

#define SH_FS              sh_enable
#define SH_OS              sh_enable
#define SH_SOCKET          sh_enable
#define SH_LINK            sh_enable
#define SH_QUEUE           sh_enable
#define SH_STACK           sh_enable
#define SH_CHAT            sh_enable

#endif



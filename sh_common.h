#include "sh_type.h"

#define SIZEOF(x)           ((SH_U32)(sizeof(x)))
#define LSTRING               (SH_S8*)

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


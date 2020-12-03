#include "sh_common.h"

extern "C" SH_BOOL sh_LittleGameInit(void);

extern "C" void sh_AppInit(void);
extern "C" void sh_AppDeInit(void);

void sh_AppInit(void)
{
    sh_LittleGameInit();
}

void sh_AppDeInit(void)
{
}


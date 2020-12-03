#include "sh_common.h"

extern "C" SH_BOOL sh_LittleGameInit(void);
extern "C" SH_BOOL sh_ToolInit(void);

extern "C" void sh_AppInit(void);
extern "C" void sh_AppDeInit(void);

void sh_AppInit(void)
{
    sh_LittleGameInit();
    sh_ToolInit();
}

void sh_AppDeInit(void)
{
}


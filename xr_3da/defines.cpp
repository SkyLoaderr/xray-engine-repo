#include "stdafx.h"

ENGINE_API BOOL bDebug	= FALSE;

// Video
u32		psCurrentMode		= 800;
u32		psCurrentBPP		= 32;
Flags32	psDeviceFlags		= {rsFullscreen|rsNoVSync|mtSound};

// textures
int		psTextureLOD		= 0;

#include "stdafx.h"

#ifdef DEBUG
	ECORE_API BOOL bDebug	= FALSE;
#endif

// Video
u32			psCurrentMode		= 800;
u32			psCurrentBPP		= 32;
Flags32		psDeviceFlags		= {rsFullscreen|rsNoVSync|mtSound};

// textures
int			psTextureLOD		= 0;

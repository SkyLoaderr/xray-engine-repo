#include "stdafx.h"

#ifdef DEBUG
	ECORE_API BOOL bDebug	= FALSE;
#endif

// Video
u32			psCurrentMode		= 800;
u32			psCurrentBPP		= 32;
Flags32		psDeviceFlags		= {rsFullscreen|rsNoVSync|rsDetails|mtPhysics|mtSound|mtNetwork};

// textures
int			psTextureLOD		= 0;

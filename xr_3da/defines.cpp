#include "stdafx.h"

ENGINE_API BOOL bDebug	= FALSE;

FS_Defs		Path_ExternalBuild =
{
	"GameData\\",
	"GameData\\levels\\",
	"GameData\\textures\\",
	"GameData\\sounds\\",
	"GameData\\cforms\\",
	"GameData\\meshes\\",
	"ScreenShots\\",
	""
};
FS_Defs		Path_InternalBuild =
{
	"\\\\x-ray\\StalkerGameData$\\",
	"\\\\x-ray\\StalkerGameData$\\levels\\",
	"\\\\x-ray\\StalkerGameData$\\textures\\",
	"\\\\x-ray\\StalkerGameData$\\sounds\\",
	"\\\\x-ray\\StalkerGameData$\\cforms\\",
	"\\\\x-ray\\StalkerGameData$\\meshes\\",
	"ScreenShots\\",
	""
};
FS_Defs		Path;

// Video
u32		psCurrentMode		= 800;
u32		psCurrentBPP		= 32;
Flags32	psDeviceFlags		= {rsFullscreen|rsNoVSync|mtSound};

// textures
int		psTextureLOD		= 0;

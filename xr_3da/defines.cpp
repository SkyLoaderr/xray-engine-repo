#include "stdafx.h"

ENGINE_API BOOL bDebug	= FALSE;

ENGINE_API FS_Defs		Path =
{
	"GameData\\levels\\",
	"GameData\\textures\\",
	"GameData\\sounds\\",
	"GameData\\cforms\\",
	"GameData\\meshes\\",
	""
};

// {0EB89097-1520-11d4-B4E3-4854E82A090D}
struct _GUID g_AppGUID		= { 0xeb89097, 0x1520, 0x11d4, { 0xb4, 0xe3, 0x48, 0x54, 0xe8, 0x2a, 0x9, 0xd } };

// Video
DWORD	psCurrentMode		= 800;
DWORD	psCurrentBPP		= 32;
DWORD	psDeviceFlags		= rsFullscreen|rsNoVSync|mtSound;

// Network
char	psName[12]			= "";

// textures
int		psTextureLOD		= 0;
DWORD	psTextureFlags		= 0;

// users
char	psUserName[64]		= "";
BOOL	psUserLevelsDone	[128];

#include "stdafx.h"

ENGINE_API const char * AVI_PATH	=		"data\\avi\\";
ENGINE_API const char * MAPS_PATH	=		"data\\maps\\";
ENGINE_API const char * SOUNDS_PATH=		"data\\sounds\\";
ENGINE_API const char * MESHES_PATH=		"data\\meshes\\";
ENGINE_API const char * CFORMS_PATH=		"data\\cforms\\";
ENGINE_API const char * SHADERS_PATH=		"data\\shaders\\";
ENGINE_API const char * CACHE_PATH=			"cache\\";
ENGINE_API const char * NAME_OF_THE_APP=	"XRAY Engine";

// {0EB89097-1520-11d4-B4E3-4854E82A090D}
struct _GUID g_AppGUID =
{ 0xeb89097, 0x1520, 0x11d4, { 0xb4, 0xe3, 0x48, 0x54, 0xe8, 0x2a, 0x9, 0xd } };

// Video
DWORD	psCurrentMode		= 512;
DWORD	psCurrentBPP		= 16;
DWORD	psDeviceFlags		= rsClearBB;	//rsFullscreen;
DWORD	psSelectedDevice	= D3DX_DEFAULT;
DWORD	psLightModel		= 0;			// lightmaps

// Network
int		psProtocol			= 0;
char	psName[12]			= "";

// textures
DWORD   psTextureFormat		= 2;				// 16 bit
DWORD	psTextureMipGen		= D3DX_FT_LINEAR;	// HighQuality
DWORD	psTextureMipFilter	= D3DTFP_LINEAR;	// Trilinear
float	psTextureContrast	= 0.5f;
float	psTextureLMContrast	= 0.7f;
float	psTextureLOD		= 0.5f;
DWORD	psTextureFlags		= 0;

// users
char	psUserName[64]	= "";
bool	psUserLevelsDone[MAX_LEVELS];

// Game
float	psIdleScale		= 1.0f;
DWORD	psNetProtocol	= 0;	// tcp/ip

float _identity[4][4] = {
{	1,	0,	0,	0 },
{	0,	1,	0,	0 },
{	0,	0,	1,	0 },
{	0,	0,	0,	1 } };

// Initialized on startup
ENGINE_API Fmatrix			precalc_identity;
ENGINE_API D3DLPOLYGON		IdentityLPolygon;
ENGINE_API Flight			gLight;


class _startup
{
public:
	_startup ();
} Startup;

_E(_startup::_startup())
	CopyMemory(&precalc_identity,_identity,16*4);

	FPcolor					d, s;
    d.set					(255, 255, 255, 255);
    s.set					(0, 0, 0, 0);

	IdentityLPolygon[0].set (0.0f, 0.0f, 0.0f, d, s, 1.0f, 1.0f);
	IdentityLPolygon[1].set (0.0f, 0.0f, 0.0f, d, s, 0.0f, 1.0f);
	IdentityLPolygon[2].set (0.0f, 0.0f, 0.0f, d, s, 0.0f, 0.0f);
	IdentityLPolygon[3].set (0.0f, 0.0f, 0.0f, d, s, 1.0f, 0.0f);

	gLight.set(D3DLIGHT_DIRECTIONAL,-1,-1,1);
	gLight.position.set	(3,3,-2);
//	gLight.position.set	(.5f,.5f,1.f);
	gLight.direction.normalize();
	gLight.diffuse.set	(1,1,1,0);
	gLight.specular.set	(1,1,1,1);
	gLight.ambient.set	(0,0,0,0);
_end;

// *****************************************************************************************
// Error handling

ENGINE_API void InterpretError(HRESULT hr, const char *file, int line)
{
    char errStr[1024], buf[2048];
    D3DXGetErrorString(hr, 1024, errStr );
    sprintf(buf,"! Engine error '%s'. File: %s, Line: %d ",errStr,file,line);
    Log(buf);
    MessageBox(NULL,buf,"Engine error",MB_OK);
        THROW;
}


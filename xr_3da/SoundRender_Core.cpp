#include "stdafx.h"
#include "soundrender_core.h"

CSoundRender_Core				SoundRender;

int		psSoundRelaxTime		= 15;
Flags32	psSoundFlags			= {ssWaveTrace | ssSoftware};
float	psSoundOcclusionScale	= 0.85f;
float	psSoundCull				= 0.07f;
float	psSoundRolloff			= 0.3f;
float	psSoundDoppler			= 0.3f;

CSoundRender_Core::CSoundRender_Core	()
{
	pListener					= NULL;
	pExtensions					= NULL;
	bPresent					= FALSE;

	// Get listener interface.
	SoundRender.pBuffer->QueryInterface( IID_IDirectSound3DListener8, (VOID**)&pListener );
	R_ASSERT					(pListener);

	// Initialize listener data
	Listener.dwSize				= sizeof(DS3DLISTENER);
	Listener.vPosition.set		( 0.0f, 0.0f, 0.0f );
	Listener.vVelocity.set		( 0.0f, 0.0f, 0.0f );
	Listener.vOrientFront.set	( 0.0f, 0.0f, 1.0f );
	Listener.vOrientTop.set		( 0.0f, 1.0f, 0.0f );
	Listener.fDistanceFactor	= 1.0f;
	Listener.fRolloffFactor		= DS3D_DEFAULTROLLOFFFACTOR;
	Listener.fDopplerFactor		= DS3D_DEFAULTDOPPLERFACTOR;
}

CSoundRender_Core::~CSoundRender_Core(void)
{
	_RELEASE		( pExtensions );
	_RELEASE		( pListener );
}

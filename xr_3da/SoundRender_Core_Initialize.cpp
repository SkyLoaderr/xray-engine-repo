#include "stdafx.h"
#include "soundrender_core.h"

CSoundManager::CSoundManager	()
{
	bPresent		= false;

	pDevice			= NULL;
	pBuffer			= NULL;
	pGeometry		= NULL;

	pCDA			= NULL;
	pSoundRender	= NULL;
	pMusicStreams	= NULL;
}

CSoundManager::~CSoundManager( )
{
}

void CSoundManager::Initialize		()
{
	Log				("Starting SOUND device...");

	InitDS3D		();

	if (bPresent) 
	{
		// 3D Sounds
		pSoundRender	= xr_new<CSoundRender>	( );

		// Streams Sounds
		pMusicStreams	= xr_new<CMusicStream>	( );
	}
	Restart			();
}

void CSoundManager::Destroy			()
{
	xr_delete		( pMusicStreams	);
	xr_delete		( pSoundRender	);
	xr_delete		( pCDA			);

	_RELEASE		( pBuffer		);
	_RELEASE		( pDevice		);
}

void CSoundManager::Restart				()
{
	// release frame/mt links
	Device.seqFrame.Remove		(this);
	Device.seqFrameMT.Remove	(this);

	if (bPresent) {
		// create _new_ ones based on values
		if (psDeviceFlags.test(mtSound))	Device.seqFrameMT.Add	(this,REG_PRIORITY_HIGH);
		else								Device.seqFrame.Add		(this,REG_PRIORITY_LOW);
	}
}

BOOL CSoundManager::CreatePrimaryBuffer	()
{
}

//-----------------------------------------------------------------------------
// Name: InitDSound()
// Desc: Creates the DSound object, primary buffer, and 3D listener
//-----------------------------------------------------------------------------
void CSoundManager::InitDS3D	( )
{
	bPresent		= false;

	if (strstr(Core.Params,"-nosound"))	return;

	if( FAILED		( DirectSoundCreate8( NULL, &pDevice, NULL ) ) )
		return;

	// Set cooperative level.
	if( FAILED		( pDevice->SetCooperativeLevel( Device.m_hWnd, DSSCL_PRIORITY ) ) )
		return;

	if( !CreatePrimaryBuffer ( ) ) return;

	bPresent		= true;
}


#include "stdafx.h"

#include "xrSound.h"

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
		pSoundRender	= new CSoundRender( );

		// Streams Sounds
		pMusicStreams	= new CMusicStream	( );
	}
	Restart			();
}

void CSoundManager::Destroy			()
{
	_DELETE			( pMusicStreams	);
	_DELETE			( pSoundRender	);
	_DELETE			( pCDA			);

	_RELEASE		( pBuffer		);
	_RELEASE		( pDevice		);
}

void CSoundManager::Restart				()
{
	// release frame/mt links
	Device.seqFrame.Remove		(this);
	Device.seqFrameMT.Remove	(this);

	if (bPresent) {
		// create new ones based on values
		if (psDeviceFlags&mtSound)	Device.seqFrameMT.Add	(this,REG_PRIORITY_HIGH);
		else						Device.seqFrame.Add		(this,REG_PRIORITY_LOW);
	}
}

BOOL CSoundManager::CreatePrimaryBuffer	()
{
	DSBUFFERDESC	dsbd;
	WAVEFORMATEX	wfm;

	// Create primary buffer.
	ZeroMemory			( &dsbd, sizeof( DSBUFFERDESC ) );
	dsbd.dwSize			= sizeof( DSBUFFERDESC );
	dsbd.dwFlags		= DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes	= 0;

	// Actual creating
	if( FAILED	( pDevice->CreateSoundBuffer( &dsbd, &pBuffer, NULL ) ) )	return FALSE;

	// Calculate primary buffer format.
	DSCAPS				dsCaps;
	dsCaps.dwSize		= sizeof(DSCAPS);
	VERIFY				(pDevice);
	CHK_DX				(pDevice->GetCaps (&dsCaps));

	ZeroMemory			( &wfm, sizeof( WAVEFORMATEX ) );
	switch ( psSoundFreq ){
		case sf_11K:	wfm.nSamplesPerSec = 11025; break;
		case sf_22K:	wfm.nSamplesPerSec = 22050; break;
		case sf_44K:	wfm.nSamplesPerSec = 44100; break;
	}
	wfm.wFormatTag		= WAVE_FORMAT_PCM;
	wfm.nChannels		= (dsCaps.dwFlags&DSCAPS_PRIMARYSTEREO)?2:1;
	wfm.wBitsPerSample	= (dsCaps.dwFlags&DSCAPS_PRIMARY16BIT)?16:8;
	wfm.nBlockAlign		= wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec	= wfm.nSamplesPerSec * wfm.nBlockAlign;

	// For safety only :)
	R_CHK(pBuffer->SetFormat	(&wfm));
	R_CHK(pBuffer->Play			(0,0,DSBPLAY_LOOPING));

	return true;
}

//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Creates the DirectSound object, primary buffer, and 3D listener
//-----------------------------------------------------------------------------
void CSoundManager::InitDS3D	( )
{
	bPresent		= false;

	if (strstr(Engine.Params,"-nosound"))	return;

	if( FAILED		( DirectSoundCreate( NULL, &pDevice, NULL ) ) )
		return;

	// Set cooperative level.
	if( FAILED		( pDevice->SetCooperativeLevel( Device.m_hWnd, DSSCL_PRIORITY ) ) )
		return;

	if( !CreatePrimaryBuffer ( ) ) return;

	bPresent		= true;
}


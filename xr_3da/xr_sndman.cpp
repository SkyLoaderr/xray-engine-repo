#include "stdafx.h"
#include <msacm.h>

#include "xr_sndman.h"
#include "xr_ini.h"
#include "xr_streamsnd.h"
#include "xr_cda.h"
#include "3DSoundRender.h"
#include "2DSoundRender.h"
#include "MusicStream.h"
#include "xr_func.h"

#include "3dsound.h"
#include "collide\cl_rapid.h"
#include "collide\cl_intersect.h"

CSoundManager*		pSounds = NULL;

// sound props
DWORD	psSoundFreq			= 0;
DWORD	psSoundModel		= 0;
float	psSoundVMaster		= 0.7f;
float	psSoundVEffects		= 1.0f;
float	psSoundVMusic		= 0.7f;
float	psSoundRolloff		= 0.5f;

//-----------------------------------------------------------------------------

CSoundManager::CSoundManager(BOOL bCDA )
{
	Log("Starting SOUND device...");
	pSounds			= this;
	
	bPresent		= false;
	
	pDevice			= NULL;
    pBuffer			= NULL;
	pGeometry		= NULL;
	
	Initialize		( );
	
	// Clear all
	pCDA			= NULL;
	pSoundRender	= NULL;
	p2DSounds		= NULL;
	pMusicStreams	= NULL;
	
	if (bPresent) {
		// CDA
		if (bCDA){
			pCDA		= new CCDA			( );
			pCDA->Open	();
		}
		
		// 3D Sounds
		pSoundRender	= new C3DSoundRender( );
		
		// 2D Sounds
		p2DSounds		= new C2DSoundRender( );
		
		// Streams Sounds
		pMusicStreams	= new CMusicStream	( );
	}
	Restart	();
}

CSoundManager::~CSoundManager( )
{
	_DELETE			( pMusicStreams	);
	_DELETE			( p2DSounds		);
	_DELETE			( pSoundRender	);
	_DELETE			( pCDA			);
	
	if (bPresent){
		//		pBuffer->SetVolume( LONG((1-fSaveWaveVol)*float(DSBVOLUME_MIN)) );
	}
	
	_RELEASE		( pBuffer		);
	_RELEASE		( pDevice		);
}

void CSoundManager::Restart()
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

BOOL CSoundManager::CreatePrimaryBuffer( )
{
    DSBUFFERDESC	dsbd;
    WAVEFORMATEX	wfm;

    // Create primary buffer.
    ZeroMemory			( &dsbd, sizeof( DSBUFFERDESC ) );
    dsbd.dwSize			= sizeof( DSBUFFERDESC );
    dsbd.dwFlags		= DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes	= 0;

	// Actual creating
    if( FAILED	( pDevice->CreateSoundBuffer( &dsbd, &pBuffer, NULL ) ) )
        return false;

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
	pBuffer->SetFormat	(&wfm);
	pBuffer->Play		(0,0,0);

	return true;
}

//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Creates the DirectSound object, primary buffer, and 3D listener
//-----------------------------------------------------------------------------
void CSoundManager::Initialize( )
{
	bPresent		= false;

    if( FAILED		( DirectSoundCreate( NULL, &pDevice, NULL ) ) )
        return;

    // Set cooperative level.
    if( FAILED		( pDevice->SetCooperativeLevel( Device.m_hWnd, DSSCL_PRIORITY ) ) )
        return;

	if( !CreatePrimaryBuffer ( ) ) return;

	bPresent		= true;

	/*
	fSaveMasterVol	= GetVMaster	( );
	LONG 			lVol;
	pBuffer->GetVolume( &lVol );
	fSaveWaveVol	= (1-float(lVol)/float(DSBVOLUME_MIN));
	pBuffer->SetVolume( DSBVOLUME_MAX );
	*/

	GetDeviceInfo	();
}

/*
BOOL TestEaxPresent( void )
{

	BOOL			bEaxOK = FALSE;		// assume it's not there till we check it is.
    HRESULT			hr;
	ULONG			ulSupport = 0;

	// if there is already a property set interface free it up before making a new one for the curent buffer
	_RELEASE		( lpPrimarySet );

	// QI for the property set interface
	if( FAILED( hr = lpSecondaryBuffer->QueryInterface( IID_IKsPropertySet,
//	if( FAILED( hr = lpdsbPrimary->QueryInterface( IID_IKsPropertySet,
						(void**)&lpPropertySet)))
	{
		_FAIL;
	}
	else
	{
		// check if our listener related property set is avalible and supports everything.
		if( FAILED( hr = lpPropertySet->QuerySupport(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_ALL, &ulSupport))){
			_RELEASE	( lpPrimarySet );
			_FAIL;
		}else{
			// check if we can get and set them.
			if ((ulSupport &(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) != (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)){
				_RELEASE( lpPrimarySet );
				_FAIL;
			}else{
				// check if our Buffer related property set is avalible and supports everything.
				if( FAILED( hr = lpPropertySet->QuerySupport(DSPROPSETID_EAXBUFFER_ReverbProperties, DSPROPERTY_EAXBUFFER_ALL, &ulSupport))){
					_RELEASE( lpPrimarySet );
					_FAIL;
				}else{
					// check if we can get and set them.
					if ((ulSupport &(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) != (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)){
						_RELEASE( lpPrimarySet );
						_FAIL;
					}else{
						// everything is avalible.
						bEaxOK = TRUE;
					}
				}
			}
		}
	}
	return(bEaxOK);
}
*/

void CSoundManager::GetDeviceInfo( )
{
	VERIFY(this);

	if ( !bPresent ) return;

	VERIFY(pDevice);
	DSCAPS				dsCaps;
	dsCaps.dwSize		= sizeof(DSCAPS);
	CHK_DX(pDevice->GetCaps  (&dsCaps));

	Log("\nDirectSound info...");
	Log("* DirectSound:                 ", (dsCaps.dwFlags&DSCAPS_EMULDRIVER)?"not available":"available.");
	Msg("* Max  HW mix buf               all: %d, stat: %d, strm: %d", dsCaps.dwMaxHwMixingAllBuffers, dsCaps.dwMaxHwMixingStaticBuffers, dsCaps.dwMaxHwMixingStreamingBuffers);
	Msg("* Free HW mix buf               all: %d, stat: %d, strm: %d", dsCaps.dwFreeHwMixingAllBuffers, dsCaps.dwFreeHwMixingStaticBuffers, dsCaps.dwFreeHwMixingStreamingBuffers);
	Msg("* Max  3D HW buf                all: %d, stat: %d, strm: %d", dsCaps.dwMaxHw3DAllBuffers, dsCaps.dwMaxHw3DStaticBuffers, dsCaps.dwMaxHw3DStreamingBuffers);
	Msg("* Free 3D HW buf                all: %d, stat: %d, strm: %d", dsCaps.dwFreeHw3DAllBuffers, dsCaps.dwFreeHw3DStaticBuffers, dsCaps.dwFreeHw3DStreamingBuffers);
	Log("* Total HW Mem kbytes:         ", dsCaps.dwTotalHwMemBytes);
	Log("* Free  HW Mem kbytes:         ", dsCaps.dwFreeHwMemBytes);
	Log("* Max   HW block size kbytes:  ", dsCaps.dwMaxContigFreeHwMemBytes);
	Log("* Max   HW transfer rate kb/s: ", dsCaps.dwUnlockTransferRateHwBuffers);
	Log("* CPU overhead SW buffers:     ", dsCaps.dwPlayCpuOverheadSwBuffers);
	Log("\n");
}
/*
void CSoundManager::SetFreq	()
{
	if (!bPresent)	return;
	clamp			( psSoundFreq, DWORD(sf_11K), DWORD(sf_44K) );

	dwFreq			= psSoundFreq;

	WAVEFORMATEX wfm;
	DWORD		 ret;
	ZeroMemory	( &wfm, sizeof( wfm ) );
	pBuffer->GetFormat( &wfm, sizeof(wfm), &ret );

	switch ( psSoundFreq ){
		case sf_11K:	wfm.nSamplesPerSec = 11025; break;
		case sf_22K:	wfm.nSamplesPerSec = 22050; break;
		case sf_44K:	wfm.nSamplesPerSec = 44100; break;
	}
	wfm.nAvgBytesPerSec		= wfm.nSamplesPerSec * wfm.nBlockAlign;
	pBuffer->SetFormat		(&wfm);
	pSoundRender->Reload	();
	p2DSounds->Reload		();
	pMusicStreams->Reload	();
	Log("* Changing freq:", wfm.nSamplesPerSec);
}
void CSoundManager::SetModel()
{
	if ( !bPresent ) return;
	clamp			( psSoundModel, DWORD(sq_DEFAULT), DWORD(sq_HIGH) );

	dwModel			= psSoundModel;
	pSoundRender->Reload	();
}
*/

void CSoundManager::SetVMusic( )
{
	if (!bPresent)	return;
	pMusicStreams->Update();
}

//-----------------------------------------------------------------------------
// Name: OnMove
//-----------------------------------------------------------------------------
void CSoundManager::OnFrame( )
{
	if (!bPresent)	return;

	Device.Statistic.Sound.Begin();
	if (Device.bActive) {
//		if (dwFreq			!= psSoundFreq)		SetFreq		();
//		if (dwModel			!= psSoundModel)	SetModel	();
		if (fMusicVolume	!= psSoundVMusic)	SetVMusic	();

		pMusicStreams->OnMove	();
		p2DSounds->OnMove		();
		pSoundRender->OnMove	();
		if (pCDA) pCDA->OnMove	();
	}
	Device.Statistic.Sound.End	();
}

//-----------------------------------------------------------------------------
void	CSoundManager::Create3D			( sound3D& S, CInifile* ini, LPCSTR section )
{
	if (!bPresent) return;
	FILE_NAME	fn;
	strcpy		(fn,ini->ReadSTRING(section, "fname"));
	char*		E = strext(fn);
	if (E)		*E = 0;
	S.handle	= pSoundRender->CreateSound(fn);
}
void	CSoundManager::Create3D			( sound3D& S, const char* fName, BOOL bCtrlFreq )
{
	if (!bPresent) return;
	FILE_NAME	fn;
	strcpy		(fn,fName);
	char*		E = strext(fn);
	if (E)		*E = 0;
	S.handle	= pSoundRender->CreateSound(fn,bCtrlFreq);
}
void	CSoundManager::Play3D			( sound3D& S, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==SND_UNDEFINED) return;
	if (S.feedback)	S.feedback->Rewind	();
	else			pSoundRender->Play	(S.handle,&S.feedback,bLoop,iLoopCnt);
}
void	CSoundManager::Play3D_Unlimited	( sound3D& S, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==SND_UNDEFINED) return;
	pSoundRender->Play	(S.handle,0,bLoop,iLoopCnt);
}
void	CSoundManager::Play3DAtPos		( sound3D& S, const Fvector &pos, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==SND_UNDEFINED) return;
	if (S.feedback)	S.feedback->Rewind	();
	else			pSoundRender->Play	(S.handle,&S.feedback,bLoop,iLoopCnt);
	S.feedback->SetPosition				(pos);
}
void	CSoundManager::Play3DAtPos_Unlimited	( sound3D& S, const Fvector &pos, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==SND_UNDEFINED) return;
	pSoundRender->Play		(S.handle,0,bLoop,iLoopCnt);
	S.feedback->SetPosition	(pos);
}
void	CSoundManager::Delete3D			( sound3D& S )
{
	if (!bPresent || S.handle==SND_UNDEFINED) {
		S.handle	= SND_UNDEFINED;
		S.feedback	= 0;
		return;
	}
	if (S.feedback)	S.feedback->Stop();
	pSoundRender->DeleteSound(S.handle);
}
//-----------------------------------------------------------------------------
int		CSoundManager::Create2D				( CInifile* ini, LPCSTR section )
{
	if (!bPresent) return SND_UNDEFINED;
	return p2DSounds->CreateSound(ini,section);
}
int		CSoundManager::Create2D				( LPCSTR fName )
{
	if (!bPresent) return SND_UNDEFINED;
	return p2DSounds->CreateSound(fName);
}
void	CSoundManager::Delete2D(int &h)
{
	if (!bPresent || h<0)	return;
	p2DSounds->DeleteSound(h);
}
C2DSound* CSoundManager::Play2D( int  hSound, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || hSound<0) return NULL;
	return p2DSounds->Play(hSound,bLoop,iLoopCnt);
}
//-----------------------------------------------------------------------------
CSoundStream* CSoundManager::CreateStream	( CInifile* ini, LPCSTR section )
{
	if (!bPresent) return NULL;
	return pMusicStreams->CreateSound(ini, section);
}
CSoundStream* CSoundManager::CreateStream	( LPCSTR fName )
{
	if (!bPresent) return NULL;
	return pMusicStreams->CreateSound(fName);
}
void	CSoundManager::DeleteStream			( CSoundStream* pSnd )
{
	if (!bPresent)	return;
	pMusicStreams->DeleteSound(pSnd);
}
//-----------------------------------------------------------------------------

BOOL CSoundManager::IsOccluded(	Fvector& P, float R, soundOccluder& occ )
{
	// Calculate RAY params
	Fvector base	= Device.vCameraPosition;
	Fvector	pos,dir;
	float	range;
	pos.random_dir	();
	pos.mul			(R);
	pos.add			(P);
	dir.sub			(pos,base);
	range = dir.magnitude();
	dir.div			(range);
	
	// 1. Check cached polygon
	float _u,_v,_range;
	if (RAPID::TestRayTri(base,dir,occ,_u,_v,_range,true))
		if (_range>0 && _range<range) return TRUE;

	// 2. Polygon doesn't picked up - real database query
	static RAPID::XRCollide	DB;
	DB.RayMode		(RAY_ONLYNEAREST);
	DB.RayPick		(0,pGeometry,base,dir,range);
	if (0==DB.GetRayContactCount()) {
		return FALSE;
	} else {
		// cache polygon
		const RAPID::raypick_info*	rpinf	= DB.GetMinRayPickInfo();
		occ[0].set	(rpinf->p[0]);
		occ[1].set	(rpinf->p[1]);
		occ[2].set	(rpinf->p[2]);
		return TRUE;
	}
}

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

CSoundManager*		pSounds = NULL;

// sound props
DWORD	psSoundFreq			= 0;
DWORD	psSoundModel		= 0;
float	psSoundVMaster		= 0.7f;
float	psSoundVEffects		= 1.0f;
float	psSoundVMusic		= 0.7f;

//-----------------------------------------------------------------------------

CSoundManager::CSoundManager(BOOL bCDA )
{
	Log("Starting SOUND device...");
	pSounds			= this;

	bPresent		= false;

	lpDirectSound	= NULL;
    lpPrimaryBuf	= NULL;

	fSaveMasterVol	= 0.0f;
	fSaveWaveVol	= 0.0f;

	// Mixer
	MMRESULT			mmr;
	MIXERLINE			mxl;
    MIXERCONTROL		mxctrl;
    MIXERLINECONTROLS	mxlc;
	UINT				uMxId;

	bVolume				= false;
	hMixer				= (HMIXER)(0);
	mmr					= mixerGetID( (HMIXEROBJ)(hMixer), &uMxId, MIXER_OBJECTF_MIXER );

	mxl.cbStruct		= sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE );

	if (mmr==MMSYSERR_NOERROR){
		mxlc.cbStruct		= sizeof(mxlc);
		mxlc.dwLineID		= mxl.dwLineID;
		mxlc.dwControlType	= MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cbmxctrl		= sizeof(MIXERCONTROL);
		mxlc.pamxctrl		= &mxctrl;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);

		if (mmr==MMSYSERR_NOERROR){
//			bVolume							= true;
			master_detail.cbStruct			= sizeof(master_detail);
			master_detail.dwControlID		= mxctrl.dwControlID;
			master_detail.cChannels			= 1;//1-для всех каналов; mxl.cChannels;
			master_detail.cMultipleItems	= mxctrl.cMultipleItems;
			master_detail.cbDetails			= sizeof(master_volume);
			master_detail.paDetails			= &master_volume;
		}
	}

	Initialize					( );

	// Clear all
	pCDA			= NULL;
	pSoundRender	= NULL;
	p2DSounds		= NULL;
	pMusicStreams	= NULL;

	if (bPresent) {
		// CDA
		if (bCDA){
			pCDA		= new CCDA( hMixer );
			pCDA->Open( );
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
	_DELETE			( p2DSounds	);
	_DELETE			( pSoundRender	);
	_DELETE			( pCDA	);

	if (bPresent){
		psSoundVMaster = fSaveMasterVol;
		SetVMaster	( );
		lpPrimaryBuf->SetVolume( LONG((1-fSaveWaveVol)*float(DSBVOLUME_MIN)) );
	}

	_RELEASE		( lpPrimaryBuf );
	_RELEASE		( lpDirectSound );
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
    if( FAILED	( lpDirectSound->CreateSoundBuffer( &dsbd, &lpPrimaryBuf, NULL ) ) )
        return false;

    // Calculate primary buffer format.
	DSCAPS				dsCaps;
	dsCaps.dwSize		= sizeof(DSCAPS);
	VERIFY				(lpDirectSound);
	CHK_DX				(lpDirectSound->GetCaps (&dsCaps));

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
	lpPrimaryBuf->SetFormat	(&wfm);
	lpPrimaryBuf->Play		(0,0,0);

	return true;
}

//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Creates the DirectSound object, primary buffer, and 3D listener
//-----------------------------------------------------------------------------
void CSoundManager::Initialize( )
{
	bPresent		= false;

    if( FAILED		( DirectSoundCreate( NULL, &lpDirectSound, NULL ) ) )
        return;

    // Set cooperative level.
    if( FAILED		( lpDirectSound->SetCooperativeLevel( Device.m_hWnd, DSSCL_PRIORITY ) ) )
        return;

	if( !CreatePrimaryBuffer ( ) ) return;

	bPresent		= true;

	fSaveMasterVol	= GetVMaster	( );
	LONG 			lVol;
	lpPrimaryBuf->GetVolume( &lVol );
	fSaveWaveVol	= (1-float(lVol)/float(DSBVOLUME_MIN));
	lpPrimaryBuf->SetVolume( DSBVOLUME_MAX );

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

	VERIFY(lpDirectSound);
	DSCAPS				dsCaps;
	dsCaps.dwSize		= sizeof(DSCAPS);
	CHK_DX(lpDirectSound->GetCaps  (&dsCaps));

	Log("\nDirectSound info...");
	Log("* DirectSound:                 ", (dsCaps.dwFlags&DSCAPS_EMULDRIVER)?"not available":"available.");
	/*
	Log("* Primary support 8 bit:       ", (dsCaps.dwFlags&DSCAPS_PRIMARY8BIT)?"yes":"no");
	Log("* Primary support 16 bit:      ", (dsCaps.dwFlags&DSCAPS_PRIMARY16BIT)?"yes":"no");
	Log("* Primary support mono:        ", (dsCaps.dwFlags&DSCAPS_PRIMARYMONO)?"yes":"no");
	Log("* Primary support stereo:	    ", (dsCaps.dwFlags&DSCAPS_PRIMARYSTEREO)?"yes":"no");
	Log("* Secondary support 8 bit:	    ", (dsCaps.dwFlags&DSCAPS_SECONDARY8BIT)?"yes":"no");
	Log("* Secondary support 16 bit:    ", (dsCaps.dwFlags&DSCAPS_SECONDARY16BIT)?"yes":"no");
	Log("* Secondary support mono:      ", (dsCaps.dwFlags&DSCAPS_SECONDARYMONO)?"yes":"no");
	Log("* Secondary support stereo:    ", (dsCaps.dwFlags&DSCAPS_SECONDARYSTEREO)?"yes":"no");
	*/
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

void CSoundManager::SetFreq( )
{
	if (!bPresent)	return;
	clamp			( psSoundFreq, DWORD(sf_11K), DWORD(sf_44K) );

	dwFreq			= psSoundFreq;

	WAVEFORMATEX wfm;
	DWORD		 ret;
	ZeroMemory	( &wfm, sizeof( wfm ) );
	lpPrimaryBuf->GetFormat( &wfm, sizeof(wfm), &ret );

	switch ( psSoundFreq ){
		case sf_11K:	wfm.nSamplesPerSec = 11025; break;
		case sf_22K:	wfm.nSamplesPerSec = 22050; break;
		case sf_44K:	wfm.nSamplesPerSec = 44100; break;
	}
	wfm.nAvgBytesPerSec	= wfm.nSamplesPerSec * wfm.nBlockAlign;
	lpPrimaryBuf->SetFormat(&wfm);
	pSoundRender->Reload	();
	p2DSounds->Reload		();
	pMusicStreams->Reload	();
	Log("* Changing freq:", wfm.nSamplesPerSec);
}

void CSoundManager::SetModel( )
{
	if ( !bPresent ) return;
	clamp			( psSoundModel, DWORD(sq_DEFAULT), DWORD(sq_HIGH) );

	dwModel			= psSoundModel;
	pSoundRender->Reload();
}

float CSoundManager::GetVMaster( )
{
	if ( !bPresent ) return 0;
	DWORD			vol = 0;
	if (bVolume)
		if (MMSYSERR_NOERROR==mixerGetControlDetails((HMIXEROBJ)hMixer, &master_detail, MIXER_GETCONTROLDETAILSF_VALUE))
			return	float(LOWORD( master_volume.dwValue ))/0xFFFF;
	return			0;
}

void CSoundManager::SetVMaster( )
{
	if (!bPresent)	return;

	if (bVolume){
		clamp			( psSoundVMaster, 0.0f, 1.0f );
		master_volume.dwValue = (int)(psSoundVMaster*0xFFFF);
		mixerSetControlDetails((HMIXEROBJ)hMixer, &master_detail, MIXER_SETCONTROLDETAILSF_VALUE);
	}
	fMasterVolume	= psSoundVMaster;
}

void CSoundManager::SetVMusic( )
{
	if (!bPresent)	return;
	if (pCDA)		pCDA->SetVolume( psSoundVMusic );
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
		if (dwFreq  != psSoundFreq)				SetFreq		();
		if (dwModel != psSoundModel)			SetModel	();
		if (fMasterVolume	!= psSoundVMaster)	SetVMaster	();
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
void	CSoundManager::LoadEnvironment	( CInifile* ini, LPCSTR section )
{
	if (!bPresent)	return;
	pSoundRender->LoadClipPlanes(ini,section);
}
void	CSoundManager::UnloadEnvironment	( )
{
	if (!bPresent)	return;
	pSoundRender->UnloadClipPlanes();
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

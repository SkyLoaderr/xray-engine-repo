// 2DSound.cpp: implementation of the C2DSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <msacm.h>

#include "2DSound.h"
#include "xr_sndman.h"
#include "xr_ini.h"

extern u32 psSoundModel;
extern u32 psSoundFreq;
extern float psSoundVEffects;
extern CSoundManager* pSounds;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DSound::C2DSound()
{
	fName		= 0;
	fVolume		= 1.f;
	fBaseVolume	= 1.f;
	fRealVolume	= 1.f;

	dwStatus	= 0;
	pBuffer		= NULL;
	bNeedUpdate	= true;
	bMustPlay	= false;
	bMustLoop	= false;
	iLoopCountRested	= 0;
}

C2DSound::~C2DSound()
{
	_RELEASE	( pBuffer );
	_FREE		( fName )
}

extern void* ParseWave(CStream *data, LPWAVEFORMATEX &wfx, u32 &len);
extern u32 Freq2Size(u32 freq);
extern void *ConvertWave(WAVEFORMATEX &wfx_dest, LPWAVEFORMATEX &wfx, void *data, u32 &dwLen);

void C2DSound::Load		(CInifile *pIni, const char *pSection)
{
	VERIFY				(pIni && pSection);

	fName				= strlwr(xr_strdup(pIni->ReadSTRING(pSection, "fname")));
	fBaseVolume			= pIni->ReadFLOAT	( pSection, "volume");
	Load				( LPSTR(0) );
}

void C2DSound::Load		(LPCSTR name)
{
	VERIFY				( pBuffer==0	);

	if (name) fName		= strlwr(xr_strdup(name));

	FILE_NAME			fn;
	sprintf				(fn,"%s%s.wav",Path.Sounds,fName);

	pBuffer				= LoadWaveAs2D( fn );
	if (!pBuffer)		THROW;
	bNeedUpdate			= true;
}

void C2DSound::Load		(C2DSound *pOriginal)
{
	fName				= xr_strdup(pOriginal->fName);
	fVolume				= 1.0f;
	fRealVolume			= 1.0f;
	dwStatus			= 0;
	pSounds->pDevice->DuplicateSoundBuffer(pOriginal->pBuffer,&pBuffer);
	VERIFY				(pBuffer);
	bNeedUpdate			= true;
}

void C2DSound::Update	()
{
	if (dwStatus&DSBSTATUS_BUFFERLOST) pBuffer->Restore();
	if (bNeedUpdate) {
		fRealVolume = .9f*fRealVolume + .1f*fVolume;
		pBuffer->SetVolume( LONG((1-fRealVolume*psSoundVEffects*fBaseVolume)*float(DSBVOLUME_MIN)) );
		bNeedUpdate = false;
	}
}

void C2DSound::OnMove	()
{
	pBuffer->GetStatus(&dwStatus);
	if ( dwStatus & DSBSTATUS_PLAYING ) {
		Update();
		if ((dwStatus&DSBSTATUS_LOOPING)&&(iLoopCountRested>0)) {
			// лупимся не вообще а определенное к-во раз
			fTimeRested -= Device.fTimeDelta;
			if ((fTimeRested<fTimeTotal)) {
				// доигрываем
				pBuffer->Stop();
				pBuffer->Play(0,0,DSBPLAY_LOCSOFTWARE);
			}
		}
	} else {
		if (bMustPlay) {
			bMustPlay	= false;
			Update		( );
			pBuffer->Play( 0, 0, DSBPLAY_LOCSOFTWARE | (bMustLoop?DSBPLAY_LOOPING:0) );
			dwStatus	|= DSBSTATUS_PLAYING;
		}
	}
}

void C2DSound::Play(BOOL bLoop, int iLoopCount)
{
	VERIFY((dwStatus & DSBSTATUS_PLAYING) == 0);
	bMustPlay = true;
	bMustLoop = bLoop;
	if (bLoop && iLoopCount) {
		fTimeRested			= fTimeTotal*iLoopCount;
		iLoopCountRested	= iLoopCount;
	} else {
		iLoopCountRested	= 0;
	}
}

void C2DSound::Stop()
{
	bMustPlay = false;
	pBuffer->Stop();
	pBuffer->SetCurrentPosition(0);
}

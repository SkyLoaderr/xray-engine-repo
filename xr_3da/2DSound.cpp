// 2DSound.cpp: implementation of the C2DSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <msacm.h>

#include "2DSound.h"
#include "xr_sndman.h"
#include "xr_ini.h"

extern DWORD psSoundModel;
extern DWORD psSoundFreq;
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

extern void* ParseWave(CStream *data, LPWAVEFORMATEX &wfx, DWORD &len);
extern DWORD Freq2Size(DWORD freq);
extern void *ConvertWave(WAVEFORMATEX &wfx_dest, LPWAVEFORMATEX &wfx, void *data, DWORD &dwLen);

LPDIRECTSOUNDBUFFER C2DSound::LoadWaveAs2D(const char *pName)
{
    DSBUFFERDESC			dsBD = {0};
	LPDIRECTSOUNDBUFFER		pBuf = NULL;

	// Fill caps structure
    dsBD.dwSize		= sizeof(dsBD);
    dsBD.dwFlags	= DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_LOCDEFER;

	// Load file into memory and parse WAV-format
	CStream *data	= new CFileStream(pName);
	WAVEFORMATEX*	pFormat;
	DWORD			dwLen;
	void *			wavedata = ParseWave(data,pFormat,dwLen);
	if (!wavedata)	{
		_DELETE(data);
		return NULL;
	}

	// Parsing OK, converting to best format
	WAVEFORMATEX			wfxdest;
	void*					converted;

//	2411252 - Andy

	pSounds->lpPrimaryBuf->GetFormat(&wfxdest,sizeof(wfxdest),0);
	if ((pFormat->wFormatTag!=1)&&(pFormat->nSamplesPerSec!=wfxdest.nSamplesPerSec)) {
		// Firstly convert to PCM with SRC freq and Channels; BPS = as Dest
		wfxdest.nChannels		= pFormat->nChannels;
		wfxdest.nSamplesPerSec	= pFormat->nSamplesPerSec;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		void *conv				= ConvertWave(wfxdest, pFormat, wavedata, dwLen);
		 _DELETE				(data);
		 if (!conv)				{_FREE(pFormat); return NULL; }

		// Secondly convert to best format for 2D
		CopyMemory				(pFormat,&wfxdest,sizeof(wfxdest));
		pSounds->lpPrimaryBuf->GetFormat(&wfxdest,sizeof(wfxdest),0);
		wfxdest.nChannels		= pFormat->nChannels;
		wfxdest.wBitsPerSample	= pFormat->wBitsPerSample;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		converted				= ConvertWave(wfxdest, pFormat, conv, dwLen);
		free					(conv);
	} else {
		// Wave has PCM format - so only one conversion
		// Freq as in PrimaryBuf, Channels = ???, Bits as in source data if possible
		if (pFormat->wFormatTag==1)	wfxdest.wBitsPerSample	= pFormat->wBitsPerSample;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		converted				= ConvertWave(wfxdest, pFormat, wavedata, dwLen);
	}
	if (!converted)				{_FREE(pFormat); return NULL; }

	dsBD.dwBufferBytes	= dwLen;
	dsBD.lpwfxFormat	= &wfxdest;

	// Creating buffer and fill it with data
    if (SUCCEEDED(pSounds->lpDirectSound->CreateSoundBuffer(&dsBD, &pBuf, NULL))){
        LPVOID	pMem1, pMem2;
        DWORD	dwSize1, dwSize2;

        if (SUCCEEDED(pBuf->Lock(0, 0, &pMem1, &dwSize1, &pMem2, &dwSize2, DSBLOCK_ENTIREBUFFER)))
        {
            CopyMemory(pMem1, converted, dwSize1);
            if ( 0 != dwSize2 )
                CopyMemory(pMem2, (char*)converted+dwSize1, dwSize2);
            pBuf->Unlock(pMem1, dwSize1, pMem2, dwSize2);

			fTimeTotal = float(dwLen) / float(wfxdest.nAvgBytesPerSec);
        } else {
			_FREE	(converted);
			_RELEASE(pBuf);
			_FREE	(pFormat);
			return NULL;
		}
	}else{
		_FREE	(converted);
		_FREE	(pFormat);
		return NULL;
	}

	// free memory
	_FREE	(converted);
	_FREE	(pFormat);
	return pBuf;
}

void C2DSound::Load		(CInifile *pIni, const char *pSection)
{
	VERIFY				(pIni && pSection);

	fName				= strlwr(strdup(pIni->ReadSTRING(pSection, "fname")));
	fBaseVolume			= pIni->ReadFLOAT	( pSection, "volume");
	Load				( LPSTR(0) );
}

void C2DSound::Load		(LPCSTR name)
{
	VERIFY				( pBuffer==0	);

	if (name) fName		= strlwr(strdup(name));

	FILE_NAME			fn;
	sprintf				(fn,"%s%s.wav",Path.Sounds,fName);

	pBuffer				= LoadWaveAs2D( fn );
	if (!pBuffer)		THROW;
	bNeedUpdate			= true;
}

void C2DSound::Load(C2DSound *pOriginal)
{
	fName				= strdup(pOriginal->fName);
	fVolume				= 1.0f;
	fRealVolume			= 1.0f;
	dwStatus			= 0;
	pSounds->lpDirectSound->DuplicateSoundBuffer(pOriginal->pBuffer,&pBuffer);
	VERIFY				(pBuffer);
	bNeedUpdate			= true;
}

void C2DSound::Update()
{
	if (dwStatus&DSBSTATUS_BUFFERLOST) pBuffer->Restore();
	if (bNeedUpdate) {
		fRealVolume = .9f*fRealVolume + .1f*fVolume;
		pBuffer->SetVolume( LONG((1-fRealVolume*psSoundVEffects*fBaseVolume)*float(DSBVOLUME_MIN)) );
		bNeedUpdate = false;
	}
}

void C2DSound::OnMove()
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

// 3DSound.cpp: implementation of the C3DSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSound.h"
#include "xr_sndman.h"

extern	DWORD			psSoundModel;
extern	DWORD			psSoundFreq;
extern	float			psSoundVEffects;
extern	CSoundManager*	pSounds;

const	DWORD			dwLoopAheadMS	= 50;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSound::C3DSound()
{
	fName		= 0;
	fVolume		= 1.f;
	fBaseVolume	= 1.f;
	fRealVolume	= 1.f;

	dwStatus	= 0;
	pBuffer		= NULL;
	pBuffer3D	= NULL;
	pExtensions	= NULL;
	ps.Init		();
	bNeedUpdate	= true;
	bMustPlay	= false;
	bMustLoop	= false;
	bCtrlFreq	= false;
	iLoopCountRested	= 0;

	owner		= 0;
}

C3DSound::~C3DSound	()
{
	_RELEASE			( pExtensions	);
	_RELEASE			( pBuffer3D		);
	_RELEASE			( pBuffer		);
	_FREE				( fName );
}

void C3DSound::Update()
{
//	if (dwStatus&DSBSTATUS_BUFFERLOST) pBuffer->Restore();
	if (bNeedUpdate) 
	{
		if (bCtrlFreq) pBuffer->SetFrequency(dwFreq);
		pBuffer3D->SetAllParameters	( ps.d3d(), DS3D_DEFERRED );
		bNeedUpdate = false;
	}
}

// Update volume
BOOL C3DSound::Update_Volume()
{
	fRealVolume			= .9f*fRealVolume + .1f*(fVolume*psSoundVEffects*fBaseVolume);
	float dist			= Device.vCameraPosition.distance_to(ps.vPosition);
	float att			= ps.flMinDistance/(psSoundRolloff*dist);	clamp(att,0.f,1.f);
	float volume		= fRealVolume*att;
	int	hw_volume		= iFloor(float(DSBVOLUME_MIN)*(1-fRealVolume));
	pBuffer->SetVolume	( hw_volume );
	return volume > psSoundCull;
}

void C3DSound::state_Process	()
{
	DWORD	dwTime			= Device.TimerAsync	();

	switch (dwState)	{
	case stStopped:
		if (bMustPlay) {
			bMustPlay			= false;
			ps.dwMode			= DS3DMODE_NORMAL;
			bNeedUpdate			= true;
			Update				( );
			dwTimeStarted		= dwTime;
			if (bMustLoop)		{
				dwTimeToStop		= dwTime + dwTimeTotal*(iLoopCount?iLoopCount:100000) - dwLoopAheadMS;
			} else {
				dwTimeToStop		= dwTime + dwTimeTotal;
			}
			if (Update_Volume())	{
				// PLAY
				pBuffer->Play		(0, 0, bMustLoop?DSBPLAY_LOOPING:0);
				dwState				= bMustLoop?stPlayingLooped:stPlaying;
			} else {
				// SIMULATE
				dwState				= bMustLoop?stSimulatingLooped:stSimulating;
			}
		} else {
			Update				( );
		}
		break;
	case stPlaying:
		if (dwTime>=dwTimeToStop)	{
			// STOP
			Stop			();
		} else {
			if (!Update_Volume()) {
				// switch to: SIMULATE
				pBuffer->Stop				();						// pause buffer
				ps.dwMode					=	DS3DMODE_DISABLE;	// disable 3D processing
				bNeedUpdate					=	true;				// signal to APPLY changes
				dwState						=	stSimulating;		// switch state
			}
		}
		Update			();
		break;
	case stSimulating:
		if (dwTime>=dwTimeToStop)	{
			// STOP
			Stop			();
		} else {
			if (Update_Volume()) {
				// switch to: PLAY
				pBuffer->SetCurrentPosition	(((dwTime-dwTimeStarted)%dwTimeTotal)*dwBytesPerMS);
				pBuffer->Play				(0, 0, 0);				// start buffer
				ps.dwMode					=	DS3DMODE_NORMAL;	// enable 3D processing
				bNeedUpdate					=	true;				// signal to APPLY changes
				dwState						=	stPlaying;			// switch state
			}
		}
		Update			();
		break;
	case stPlayingLooped:
		if (dwTime>=dwTimeToStop)	{
			// Ordinary play
			pBuffer->Play					(0,0,0);				// Override flags - play up to the end of buffer
			dwTimeToStop					+=	dwLoopAheadMS;
			dwState							=	stPlaying;			// switch state
		} else {
			if (!Update_Volume()) {
				// switch to: SIMULATE
				pBuffer->Stop				();						// pause buffer
				ps.dwMode					=	DS3DMODE_DISABLE;	// disable 3D processing
				bNeedUpdate					=	true;				// signal to APPLY changes
				dwState						=	stSimulatingLooped;	// switch state
			}
		}
		Update			();
		break;
	case stSimulatingLooped:
		if (dwTime>=dwTimeToStop)	{
			// Ordinary simulate
			dwTimeToStop					+=	dwLoopAheadMS;
			dwState							=	stSimulating;		// switch state
		} else {
			if (Update_Volume()) {
				// switch to: PLAY
				pBuffer->SetCurrentPosition	(((dwTime-dwTimeStarted)%dwTimeTotal)*dwBytesPerMS);
				pBuffer->Play				(0, 0, DSBPLAY_LOOPING);// start buffer
				ps.dwMode					=	DS3DMODE_NORMAL;	// enable 3D processing
				bNeedUpdate					=	true;				// signal to APPLY changes
				dwState						=	stPlaying;			// switch state
			}
		}
		Update			();
		break;
	}
}

void C3DSound::SetPosition	(const Fvector &pos)
{
	ps.vPosition.set	(pos);
	bNeedUpdate			= true;
}

void C3DSound::SetFrequency	(DWORD freq)
{
	dwFreq				= freq;
	bNeedUpdate			= true;
}

void C3DSound::SetMinMax	(float min, float max)
{
	ps.flMinDistance	= min;
	ps.flMaxDistance	= max;
	bNeedUpdate			= true;
}

void C3DSound::Play			(C3DSound** P, BOOL bLoop, int LoopCount)
{
	VERIFY((dwStatus & DSBSTATUS_PLAYING) == 0);
	owner		= P;
	bMustPlay	= true;
	iLoopCount	= LoopCount;
	bMustLoop	= bLoop?((1==LoopCount)?FALSE:TRUE);
}

void C3DSound::Stop			()
{
	bMustPlay					= false;
	pBuffer->Stop				();
	pBuffer->SetCurrentPosition	(0);
	if (owner)					{ *owner = 0; owner	= 0; }
	ps.dwMode					= DS3DMODE_DISABLE;
	dwState						= stStopped;
	bNeedUpdate					= true;
}

void C3DSound::Rewind		()
{
	pBuffer->SetCurrentPosition(0);
	bNeedUpdate = true;
}

// 3DSound.cpp: implementation of the C3DSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSound.h"
#include "xr_sndman.h"

extern DWORD psSoundModel;
extern DWORD psSoundFreq;
extern float psSoundVEffects;
extern CSoundManager* pSounds;

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
	if (dwStatus&DSBSTATUS_BUFFERLOST) pBuffer->Restore();
	if (bNeedUpdate) 
	{
		if (bCtrlFreq) pBuffer->SetFrequency(dwFreq);
		fRealVolume = .9f*fRealVolume + .1f*(fVolume*psSoundVEffects*fBaseVolume);
		pBuffer3D->SetAllParameters	( ps.d3d(), DS3D_DEFERRED );
		bNeedUpdate = false;
	}
}

// Update volume
void C3DSound::Update_Volume()
{
	fRealVolume			= .9f*fRealVolume + .1f*(fVolume*psSoundVEffects*fBaseVolume);
	float dist			= Device.vCameraPosition.distance_to(ps.vPosition);
	float att			= ps.flMinDistance/(psSoundRolloff*dist);	clamp(att,0.f,1.f);
	float volume		= fRealVolume*att;

	int	hw_volume		= iFloor(float(DSBVOLUME_MIN)*(1-fRealVolume));
	pBuffer->SetVolume	( hw_volume );
}

void C3DSound::OnMove()
{
	// Get status
	DWORD old_Status	= dwStatus;
	pBuffer->GetStatus	(&dwStatus);

	// Logic
	if ( dwStatus & DSBSTATUS_PLAYING ) 
	{
		Update();
		if ((dwStatus&DSBSTATUS_LOOPING)&&(iLoopCountRested>0)) {
			// лупимся не вообще а определенное к-во раз
			fTimeRested -= Device.fTimeDelta;
			if ((fTimeRested<fTimeTotal)) {
				// доигрываем
				pBuffer->Stop();
				pBuffer->Play(0,0,0);
			}
		}
	} else {
		if (bMustPlay) {
			bMustPlay	= false;
			ps.dwMode	= DS3DMODE_NORMAL;
			Update		( );
			pBuffer->Play( 0, 0, (bMustLoop?DSBPLAY_LOOPING:0));
			dwStatus	|= DSBSTATUS_PLAYING;
		} else {
			if (old_Status&DSBSTATUS_PLAYING) 
			{
				// The sound-play has ended
				internalStopOrComplete	();
			}
		}
	}
	Update	();
}

void C3DSound::internalStopOrComplete	()
{
	if (owner)			{ *owner = 0; owner	= 0; }
	ps.dwMode			=	DS3DMODE_DISABLE;
	dwStatus			&= ~DSBSTATUS_PLAYING;
	bNeedUpdate			= true;
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

void C3DSound::Play			(C3DSound** P, BOOL bLoop, int iLoopCount)
{
	VERIFY((dwStatus & DSBSTATUS_PLAYING) == 0);
	owner		= P;
	bMustPlay	= true;
	bMustLoop	= bLoop;
	if (bLoop && iLoopCount) 
	{
		fTimeRested			= fTimeTotal*iLoopCount;
		iLoopCountRested	= iLoopCount;
	} else {
		iLoopCountRested	= 0;
	}
}

void C3DSound::Stop			()
{
	bMustPlay = false;
	pBuffer->Stop				();
	pBuffer->SetCurrentPosition	(0);
	internalStopOrComplete		();
}

void C3DSound::Rewind		()
{
	pBuffer->SetCurrentPosition(0);
	bNeedUpdate = true;
}

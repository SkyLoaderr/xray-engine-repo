// 3DSound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSound.h"
#include "xr_sndman.h"
#include "xr_creator.h"
#include "xr_object.h"
#include "feel_sound.h"

extern	u32			psSoundModel;
extern	u32			psSoundFreq;
extern	float			psSoundVEffects;
extern	CSoundManager*	pSounds;

const	u32			dwLoopAheadMS	= 75;
const	float			fOcclusionSpeed	= 1.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSound::CSound	(BOOL b_3D)
{
	fName		= 0;
	fVolume		= 1.f;
	fBaseVolume	= 1.f;
	fRealVolume	= 1.f;

	dwState 	= stStopped;
	pBuffer		= NULL;
	pBuffer3D	= NULL;
	pExtensions	= NULL;
	ps.Init		();
	bNeedUpdate	= true;
	bMustPlay	= false;
	bMustLoop	= false;
	iLoopCount	= 0;

	_3D			= b_3D;
	_Freq		= false;

	owner		= 0;
}

CSound::~CSound	()
{
	_RELEASE			( pExtensions	);
	_RELEASE			( pBuffer3D		);
	_RELEASE			( pBuffer		);
	_FREE				( fName			);
}

// Update params
void CSound::Update_Params()
{
	if (bNeedUpdate) 
	{
		bNeedUpdate		= false;
		if (_Freq)		pBuffer->SetFrequency			( dwFreq );
		if (_3D)		pBuffer3D->SetAllParameters		( ps.d3d(), DS3D_DEFERRED ); 
		else			{
			int	hw_volume		= iFloor(float(DSBVOLUME_MIN)*(1-fVolume*psSoundVEffects));
			pBuffer->SetVolume	( hw_volume );
		}
	}
}

// Update volume
BOOL CSound::Update_Volume()
{
	if (!_3D)			return	TRUE;

	float	dist		= Device.vCameraPosition.distance_to(ps.vPosition);
	if (dist>ps.flMaxDistance)	return FALSE;
	else {
		float att			= ps.flMinDistance/(psSoundRolloff*dist);	clamp(att,0.f,1.f);
		if (att*fVolume*psSoundVEffects>psSoundCull)	
		{
			Update_Occlusion	();
			
			float vol_occ		= fBaseVolume*(1.f-psSoundOcclusionScale)+psSoundOcclusionScale;
			fRealVolume			= .9f*fRealVolume + .1f*(fVolume*psSoundVEffects*vol_occ);
			float volume		= fRealVolume*att;
			if (volume>psSoundCull)	{
				int	hw_volume		= iFloor(float(DSBVOLUME_MIN)*(1-fRealVolume));
				pBuffer->SetVolume	( hw_volume );
				return TRUE;
			} else {
				return FALSE;
			}
		} else {
			return FALSE;
		}
	}
}

void CSound::Update_Occlusion	()
{
	if (!_3D)			return;

	if (pSounds->IsOccluded(ps.vPosition,1.f,occluder))	{
		fBaseVolume -=	Device.fTimeDelta*fOcclusionSpeed;
		clamp		(fBaseVolume,0.f,1.f);
	} else {
		fBaseVolume +=	Device.fTimeDelta*fOcclusionSpeed;
		clamp		(fBaseVolume,0.f,1.f);
	}
}

void CSound::PropagadeEvent()
{
	dwTimeToPropagade		+= soundEventPulse;
	if (0==owner)			return;
	if (0==owner->g_type)	return;
	if (0==pCreator)		return;

	// Calculate range
	float	limitV			= .01f;
	float	clip			= (ps.flMinDistance*fVolume) / (psSoundRolloff*limitV); // (Dmin*V)/(R*V')
	float	range			= _min(ps.flMaxDistance,clip);
	if	(clip<0)			return;

	// Query objects
	pCreator->ObjectSpace.GetNearest	(ps.vPosition,range);

	// Iterate
	CObjectSpace::NL_IT		it	= pCreator->ObjectSpace.q_nearest.begin	();
	CObjectSpace::NL_IT		end	= pCreator->ObjectSpace.q_nearest.end	();
	for (; it!=end; it++)
	{
		CObject*	O		= *it;
		Feel::Sound* L		= dynamic_cast<Feel::Sound*>(O);
		if (0==L)			continue;

		// Energy and signal
		Fvector				Oc;
		O->clCenter			(Oc);
		float D				= ps.vPosition.distance_to(Oc);
		float A				= ps.flMinDistance/(psSoundRolloff*D);					// (Dmin*V)/(R*D) 
		clamp				(A,0.f,1.f);
		float Power			= A*fVolume;
		if (Power>EPS_S)	L->feel_sound_new	(owner->g_object,owner->g_type,ps.vPosition,Power);
	}
}

void CSound::OnMove		(vector<sound_defer>& plist)
{
	u32	dwTime			= Device.TimerAsync	();

	switch (dwState)	
	{
	case stStopped:
		if (bMustPlay) {
			bMustPlay			= false;
			ps.dwMode			= _3D?DS3DMODE_NORMAL:DS3DMODE_DISABLE;
			bNeedUpdate			= true;
			Update_Params		();
			dwTimeStarted		= dwTime;
			if (bMustLoop)		{
				dwTimeToStop		= dwTime + dwTimeTotal*(iLoopCount?iLoopCount:100000) - dwLoopAheadMS;
			} else {
				dwTimeToStop		= dwTime + dwTimeTotal;
			}
			if (Update_Volume())	{
				// PLAY
				plist.push_back		(sound_defer(pBuffer, bMustLoop?DSBPLAY_LOOPING:0));
				dwState				= bMustLoop?stPlayingLooped:stPlaying;
			} else {
				// SIMULATE
				dwState				= bMustLoop?stSimulatingLooped:stSimulating;
			}
			dwTimeToPropagade		= dwTime;
			PropagadeEvent			();
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
			if (dwTime>=dwTimeToPropagade)		PropagadeEvent();
		}
		break;
	case stSimulating:
		if (dwTime>=dwTimeToStop)	{
			// STOP
			Stop			();
		} else {
			if (Update_Volume()) {
				// switch to: PLAY
				pBuffer->SetCurrentPosition	(((dwTime-dwTimeStarted)%dwTimeTotal)*dwBytesPerMS);
				plist.push_back				(sound_defer(pBuffer, 0));	// start buffer
				ps.dwMode					=	_3D?DS3DMODE_NORMAL:DS3DMODE_DISABLE;	// enable 3D processing
				bNeedUpdate					=	true;				// signal to APPLY changes
				dwState						=	stPlaying;			// switch state
			}
			if (dwTime>=dwTimeToPropagade)		PropagadeEvent();
		}
		break;
	case stPlayingLooped:
		if (dwTime>=dwTimeToStop)	{
			// Ordinary play
			plist.push_back					(sound_defer(pBuffer, 0));	// Override flags - play up to the end of buffer
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
		if (dwTime>=dwTimeToPropagade)		PropagadeEvent();
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
				plist.push_back				(sound_defer(pBuffer, DSBPLAY_LOOPING));	// start buffer
				ps.dwMode					=	_3D?DS3DMODE_NORMAL:DS3DMODE_DISABLE;	// enable 3D processing
				bNeedUpdate					=	true;				// signal to APPLY changes
				dwState						=	stPlayingLooped;	// switch state
			}
		}
		if (dwTime>=dwTimeToPropagade)		PropagadeEvent();
		break;
	}
	Update_Params		();
}

void CSound::SetPosition	(const Fvector &pos)
{
	ps.vPosition.set	(pos);
	bNeedUpdate			= true;
}

void CSound::SetFrequency	(u32 freq)
{
	dwFreq				= freq;
	clamp				(dwFreq,u32(DSBFREQUENCY_MIN),u32(DSBFREQUENCY_MAX));
	bNeedUpdate			= true;
}
void CSound::SetFrequencyScale(float S)
{
	dwFreq				= iFloor(float(dwFreqBase)*S);
	clamp				(dwFreq,u32(DSBFREQUENCY_MIN),u32(DSBFREQUENCY_MAX));
	bNeedUpdate			= true;
}
void CSound::SetMinMax		(float min, float max)
{
	ps.flMinDistance	= min;
	ps.flMaxDistance	= max;
	bNeedUpdate			= true;
}

void CSound::Play			(sound* P, BOOL bLoop, int LoopCount)
{
	R_ASSERT			(dwState == stStopped);
	owner				= P;
	bMustPlay			= true;
	iLoopCount			= LoopCount;
	bMustLoop			= bLoop?((1==LoopCount)?FALSE:TRUE):FALSE;
	bNeedUpdate			= true;
}

void CSound::Stop			()
{
	bMustPlay					= false;
	pBuffer->Stop				();
	pBuffer->SetCurrentPosition	(0);
	if (owner)					{ owner->feedback = 0; owner	= 0; }
	ps.dwMode					= DS3DMODE_DISABLE;
	dwState						= stStopped;
	bNeedUpdate					= true;
}

void CSound::Rewind			()
{
	pBuffer->SetCurrentPosition	(0);
	u32 dwTime				= Device.TimerAsync();
	u32 dwDiff				= dwTime-dwTimeStarted;
	dwTimeStarted				+= dwDiff;
	dwTimeToStop				+= dwDiff;
	dwTimeToPropagade			= dwTime;
	bNeedUpdate					= true;
}

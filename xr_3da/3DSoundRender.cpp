// 3DSoundRender.cpp: implementation of the CSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSoundRender.h"
#include "3DSound.h"
#include "device.h"
#include "xr_sndman.h"
#include "xr_ini.h"

int		psSoundRelaxTime		= 3;
u32	psSoundFlags			= ssWaveTrace;
float	psSoundOcclusionScale	= 0.85f;
float	psSoundCull				= 0.07f;
float	psSoundRolloff			= 0.3f;
float	psSoundDoppler			= 0.3f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundRender::CSoundRender()
{
	pListener					= NULL;
	pExtensions					= NULL;

    // Get listener interface.
	R_ASSERT					(pSounds);
	R_ASSERT					(pSounds->pBuffer);
    pSounds->pBuffer->QueryInterface( IID_IDirectSound3DListener, (VOID**)&pListener );
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

CSoundRender::~CSoundRender()
{
	_RELEASE		( pExtensions );
	_RELEASE		( pListener );
}

void CSoundRender::OnMove()
{
	// Update states
	defer.clear	();
	for (u32 i=0; i<sounds.size(); i++) 
	{
		for (u32 j=0; j<sounds[i].size(); j++) 
		{
			CSound *pSnd = sounds[i][j];
			pSnd->OnMove		(defer);
			if (pSnd->isPlaying	()) 
			{
				Device.Statistic.dwSND_Played++;
				pSnd->dwLastTimeActive = Device.dwTimeGlobal;
			} else {
				if (j && (Device.dwTimeGlobal-pSnd->dwLastTimeActive) > u32(psSoundRelaxTime*1000))
				{
					_DELETE			(pSnd);
					sounds[i].erase	(sounds[i].begin()+j);
					j--;
				}
			}
		}
		Device.Statistic.dwSND_Allocated+=sounds[i].size();
	}

	// Update listener
	Listener.vVelocity.sub				(Device.vCameraPosition, Listener.vPosition );
	Listener.vVelocity.div				(Device.fTimeDelta);
	Listener.vPosition.set				(Device.vCameraPosition);
	Listener.vOrientFront.set			(Device.vCameraDirection);
	Listener.vOrientTop.set				(Device.vCameraTop);
	Listener.fDopplerFactor				= psSoundDoppler;
	Listener.fRolloffFactor				= psSoundRolloff;
	pListener->SetAllParameters			((DS3DLISTENER*)&Listener, DS3D_DEFERRED );
	pListener->CommitDeferredSettings	();

	// Start deferred sounds
	for (vector<sound_defer>::iterator it=defer.begin(); it!=defer.end(); it++)
		it->P->Play	(0,0,it->F);
}

int CSoundRender::FindByName			(LPCSTR name, BOOL _3D, BOOL _Freq) {
	for (u32 i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			if ((stricmp(sounds[i][0]->fName,name)==0)&&(_3D==sounds[i][0]->_3D)&&(_Freq==sounds[i][0]->_Freq))  
				return i;
		}
	}
	return -1;
}

int CSoundRender::FindEmptySlot		()
{
	for (u32 i=0; i<sounds.size(); i++) {
		if (sounds[i].size()==0) return i;
	}
	return -1;
}

int	CSoundRender::Append				(CSound *p)
{
	// empty slot not found - expand lists
	vector <CSound *>	pv;
	sounds.push_back( pv );
	refcounts.push_back(1);
	int i = sounds.size()-1;
	sounds[i].push_back(p);
	return i;
}

int	CSoundRender::CreateSound			(LPCSTR name, BOOL _3D, BOOL _Freq, BOOL bNotClip)
{
	int fnd;
	if ((fnd=FindByName(name,_3D, _Freq))>=0) {
		refcounts[fnd]	+=1;
		return fnd;
	}

	// sound not found - create new
	CSound *pSnd	= new CSound(_3D);
	pSnd->Load		(name,_Freq);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

int	CSoundRender::CreateSound			(CInifile *pIni, LPCSTR section)
{
	FILE_NAME	fn;
	strcpy(fn,pIni->ReadSTRING(section, "fname"));
	BOOL _3D	= pIni->LineExists(section, "ctrl_3d");
	BOOL _Freq	= pIni->LineExists(section, "ctrl_freq");
	
	int fnd;
	if ((fnd=FindByName(fn,_3D,_Freq))>=0) {
		refcounts[fnd]+=1;
		return fnd;
	}

	// sound not found - create new
	CSound *pSnd = new CSound		(_3D);
	pSnd->Load							(pIni,section);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

void CSoundRender::DeleteSound		(int& hSound) 
{
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	VERIFY(refcounts[hSound]>0);
	refcounts[hSound]-=1;
	if (refcounts[hSound]==0) {
		// all references destroyed - destroy sound as itself
		for (u32 i=0; i<sounds[hSound].size(); i++)
		{
			_DELETE(sounds[hSound][i]);
		}
		sounds[hSound].clear();
	}
	hSound = -1;
}

CSound* CSoundRender::GetFreeSound(int hSound) 
{
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	for (u32 i=0; i<sounds[hSound].size(); i++)
	{
		if (!sounds[hSound][i]->isPlaying())
			return sounds[hSound][i];
	}
	// xr_free sound not found - create duplicate
	CSound *pSnd = new CSound	(sounds[hSound].front()->_3D);
	pSnd->Load						(sounds[hSound].front());
	sounds[hSound].push_back		(pSnd);
	return pSnd;
}

void CSoundRender::Play(int hSound, sound* P, BOOL bLoop, int iLoopCnt)
{
	P->feedback			= GetFreeSound	(hSound);
	P->feedback->Play	(P, bLoop, iLoopCnt);
}

void CSoundRender::Reload()
{
	for (u32 i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			// Main sound
			sounds[i][0]->Stop();
			_RELEASE(sounds[i][0]->pBuffer3D);
			_RELEASE(sounds[i][0]->pBuffer);
			sounds[i][0]->Load(LPSTR(0));
			sounds[i][0]->bNeedUpdate = true;
			if (sounds[i][0]->isPlaying()) {
				sounds[i][0]->Play(sounds[i][0]->owner,sounds[i][0]->bMustLoop, sounds[i][0]->iLoopCount);
			}

			// Clones
			for (u32 j=1; j<sounds[i].size(); j++) {
				sounds[i][j]->Stop();
				_RELEASE(sounds[i][j]->pBuffer3D);
				_RELEASE(sounds[i][j]->pBuffer);
				pSounds->pDevice->DuplicateSoundBuffer(sounds[i][0]->pBuffer,&sounds[i][j]->pBuffer);
				VERIFY	(sounds[i][j]->pBuffer);
				sounds[i][j]->pBuffer->QueryInterface(IID_IDirectSound3DBuffer,(void **)(&sounds[i][j]->pBuffer3D));
				sounds[i][j]->bNeedUpdate = true;
				if (sounds[i][j]->isPlaying()) {
					sounds[i][j]->Play(sounds[i][j]->owner,sounds[i][j]->bMustLoop, sounds[i][j]->iLoopCount);
				}
			}
		}
	}
}

// 3DSoundRender.cpp: implementation of the C3DSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSoundRender.h"
#include "3DSound.h"
#include "device.h"
#include "xr_sndman.h"
#include "xr_ini.h"

int		psSoundRelaxTime		= 3;
DWORD	psSoundFlags			= ssWaveTrace;
float	psSoundOcclusionScale	= 0.85f;
float	psSoundCull				= 0.07f;
float	psSoundRolloff			= 0.3f;
float	psSoundDoppler			= 0.3f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSoundRender::C3DSoundRender()
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

C3DSoundRender::~C3DSoundRender()
{
	_RELEASE		( pExtensions );
	_RELEASE		( pListener );
}

void C3DSoundRender::OnMove()
{
	for (DWORD i=0; i<sounds.size(); i++) 
	{
		for (DWORD j=0; j<sounds[i].size(); j++) 
		{
			C3DSound *pSnd = sounds[i][j];
			pSnd->OnMove		();
			if (pSnd->isPlaying	()) 
			{
				Device.Statistic.dwSND_Played++;
				pSnd->dwLastTimeActive = Device.dwTimeGlobal;
			} else {
				if (j && (Device.dwTimeGlobal-pSnd->dwLastTimeActive) > DWORD(psSoundRelaxTime*1000))
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
}

int C3DSoundRender::FindByName			(LPCSTR name, BOOL bFreq) {
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			if ((strcmp(sounds[i][0]->fName,name)==0)&&(bFreq==sounds[i][0]->bCtrlFreq))  return i;
		}
	}
	return -1;
}

int C3DSoundRender::FindEmptySlot		()
{
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()==0) return i;
	}
	return -1;
}

int	C3DSoundRender::Append				(C3DSound *p)
{
	// empty slot not found - expand lists
	vector <C3DSound *>	pv;
	sounds.push_back( pv );
	refcounts.push_back(1);
	int i = sounds.size()-1;
	sounds[i].push_back(p);
	return i;
}

int	C3DSoundRender::CreateSound			(LPCSTR name, BOOL bCtrlFreq, BOOL bNotClip)
{
	int fnd;
	if ((fnd=FindByName(name,bCtrlFreq))>=0) {
		refcounts[fnd]+=1;
		return fnd;
	}

	// sound not found - create new
	C3DSound *pSnd = new C3DSound;
	pSnd->Load(name,bCtrlFreq);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

int	C3DSoundRender::CreateSound			(CInifile *pIni, LPCSTR section)
{
	FILE_NAME	fn;
	strcpy(fn,pIni->ReadSTRING(section, "fname"));
	BOOL bCtrlFreq	= pIni->LineExists(section, "ctrl_freq");
	
	int fnd;
	if ((fnd=FindByName(fn,bCtrlFreq))>=0) {
		refcounts[fnd]+=1;
		return fnd;
	}

	// sound not found - create new
	C3DSound *pSnd = new C3DSound;
	pSnd->Load(pIni,section);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

void C3DSoundRender::DeleteSound		(int& hSound) 
{
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	VERIFY(refcounts[hSound]>0);
	refcounts[hSound]-=1;
	if (refcounts[hSound]==0) {
		// all references destroyed - destroy sound as itself
		for (DWORD i=0; i<sounds[hSound].size(); i++)
		{
			_DELETE(sounds[hSound][i]);
		}
		sounds[hSound].clear();
	}
	hSound = -1;
}

C3DSound* C3DSoundRender::GetFreeSound(int hSound) 
{
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	for (DWORD i=0; i<sounds[hSound].size(); i++)
	{
		if (!sounds[hSound][i]->isPlaying())
			return sounds[hSound][i];
	}
	// free sound not found - create duplicate
	C3DSound *pSnd = new C3DSound;
	pSnd->Load(sounds[hSound][0]);
	sounds[hSound].push_back(pSnd);
	return pSnd;
}

void C3DSoundRender::Play(int hSound, sound3D* P, BOOL bLoop, int iLoopCnt)
{
	P->feedback			= GetFreeSound	(hSound);
	P->feedback->Play	(P, bLoop, iLoopCnt);
}

void C3DSoundRender::Reload()
{
	for (DWORD i=0; i<sounds.size(); i++) {
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
			for (DWORD j=1; j<sounds[i].size(); j++) {
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

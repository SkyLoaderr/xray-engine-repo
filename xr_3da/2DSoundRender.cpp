// 2DSoundRender.cpp: implementation of the C2DSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "2DSoundRender.h"
#include "2DSound.h"
#include "device.h"
#include "xr_sndman.h"
#include "xr_ini.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DSoundRender::C2DSoundRender()
{
}

C2DSoundRender::~C2DSoundRender()
{
}

void C2DSoundRender::OnMove()
{
	for (DWORD i=0; i<sounds.size(); i++) {
		for (DWORD j=0; j<sounds[i].size(); j++) {
			sounds[i][j]->OnMove();
		}
	}
}

int C2DSoundRender::FindByName(LPCSTR name) {
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			if (strcmp(sounds[i][0]->fName,name)==0)  return i;
		}
	}
	return -1;
}
int C2DSoundRender::FindEmptySlot()
{
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()==0) return i;
	}
	return -1;
}

int	C2DSoundRender::Append(C2DSound *p)
{
	// empty slot not found - expand lists
	vector <C2DSound *>	pv;
	sounds.push_back( pv );
	refcounts.push_back(1);
	int i = sounds.size()-1;
	sounds[i].push_back(p);
	return i;
}

int C2DSoundRender::CreateSound(CInifile *pIni, LPCSTR section)
{
	int			fnd;
	FILE_NAME	fn;
	strcpy		(fn,pIni->ReadSTRING(section, "fname"));

	if ((fnd=FindByName(fn))>=0) {
		refcounts[fnd]+=1;
		return fnd;
	}

	// sound not found - create new
	C2DSound *pSnd = new C2DSound;
	pSnd->Load(pIni, section);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

int	C2DSoundRender::CreateSound(LPCSTR name)
{
	int fnd;
	if ((fnd=FindByName(name))>=0) {
		refcounts[fnd]+=1;
		return fnd;
	}

	// sound not found - create new
	C2DSound *pSnd = new C2DSound;
	pSnd->Load(name);

	// search for empty slot
	if ((fnd=FindEmptySlot())>=0) {
		// empty slot
		sounds[fnd].push_back(pSnd);
		refcounts[fnd]+=1;
		return fnd;
	}
	return Append(pSnd);
}

void C2DSoundRender::DeleteSound(int& hSound) {
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

C2DSound* C2DSoundRender::GetFreeSound(int hSound) {
	VERIFY(hSound>=0);
	VERIFY(hSound<int(sounds.size()));
	for (DWORD i=0; i<sounds[hSound].size(); i++)
	{
		if (!sounds[hSound][i]->isPlaying())
			return sounds[hSound][i];
	}
	// free sound not found - create duplicate
	C2DSound *pSnd = new C2DSound;
	pSnd->Load(sounds[hSound][0]);
	sounds[hSound].push_back(pSnd);
	return pSnd;
}

C2DSound* C2DSoundRender::Play(int hSound, BOOL bLoop, int iLoopCnt)
{
	C2DSound *pSnd = GetFreeSound(hSound);
	pSnd->Play(bLoop, iLoopCnt);
	return pSnd;
}

void C2DSoundRender::Reload()
{
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			// Main sound
			sounds[i][0]->Stop();
			_RELEASE(sounds[i][0]->pBuffer);
			sounds[i][0]->Load((LPCSTR)0);
			sounds[i][0]->bNeedUpdate = true;
			if (sounds[i][0]->isPlaying()) {
				sounds[i][0]->Play(sounds[i][0]->bMustLoop, sounds[i][0]->iLoopCountRested);
			}

			// Clones
			for (DWORD j=1; j<sounds[i].size(); j++) {
				sounds[i][j]->Stop();
				_RELEASE(sounds[i][j]->pBuffer);
				pSounds->lpDirectSound->DuplicateSoundBuffer(sounds[i][0]->pBuffer,&sounds[i][j]->pBuffer);
				VERIFY	(sounds[i][j]->pBuffer);
				sounds[i][j]->bNeedUpdate = true;
				if (sounds[i][j]->isPlaying()) {
					sounds[i][j]->Play(sounds[i][j]->bMustLoop, sounds[i][j]->iLoopCountRested);
				}
			}
		}
	}
}

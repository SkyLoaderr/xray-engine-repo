// 3DSoundRender.cpp: implementation of the C3DSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSoundRender.h"
#include "3DSound.h"
#include "device.h"
#include "xr_sndman.h"
#include "xr_ini.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSoundRender::C3DSoundRender()
{
	pListener		= NULL;
	pExtensions		= NULL;

    // Get listener interface.
    pSounds->lpPrimaryBuf->QueryInterface( IID_IDirectSound3DListener, (VOID**)&pListener );
	Listener.Init	( );
}

C3DSoundRender::~C3DSoundRender()
{
	_RELEASE		( pExtensions );
	_RELEASE		( pListener );
}

/*
void FIntersectionQuad::Render()
{

}

void C3DSoundRender::RenderClipPlanes()
{
	Fmaterial mmx;
	mmx.set(1,1,1);
	mmx.diffuse.a=.5f;
	HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d());
//	HW.pDevice->SetMaterial(mmx.d3d());
	Device.rsSet(vZTest|vAlpha);
	int hTexture = Device.Texture.Create("STD:refl","std_aop",false);
	Device.Texture.Set(hTexture);
	for (int k=0; k<clip_planes.size(); k++) {
		clip_planes[k].Render();
	}
	Device.Texture.Delete(hTexture);
}
*/

const DWORD dwSndKillTime = 3;
void C3DSoundRender::OnMove()
{
//	Log("*****");
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
				if (j && (Device.dwTimeGlobal-pSnd->dwLastTimeActive) > dwSndKillTime*1000)
				{
					_DELETE(pSnd);
					sounds[i].erase(sounds[i].begin()+j);
					j--;
				}
			}
		}
		Device.Statistic.dwSND_Allocated+=sounds[i].size();
	}

	Listener.vVelocity.sub		(Device.vCameraPosition, Listener.vPosition );
	Listener.vVelocity.div		(Device.fTimeDelta);
	Listener.vPosition.set		(Device.vCameraPosition);
	Listener.vOrientFront.set	(Device.vCameraDirection);
	Listener.vOrientTop.set		(Device.vCameraTop);
	VERIFY(pListener);
	pListener->SetAllParameters	(Listener.d3d(), DS3D_DEFERRED );
	pListener->CommitDeferredSettings( );
}

int C3DSoundRender::FindByName(LPCSTR name, BOOL bFreq) {
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()) {
			if ((strcmp(sounds[i][0]->fName,name)==0)&&(bFreq==sounds[i][0]->bCtrlFreq))  return i;
		}
	}
	return -1;
}
int C3DSoundRender::FindEmptySlot()
{
	for (DWORD i=0; i<sounds.size(); i++) {
		if (sounds[i].size()==0) return i;
	}
	return -1;
}

int	C3DSoundRender::Append(C3DSound *p)
{
	// empty slot not found - expand lists
	vector <C3DSound *>	pv;
	sounds.push_back( pv );
	refcounts.push_back(1);
	int i = sounds.size()-1;
	sounds[i].push_back(p);
	return i;
}

int	C3DSoundRender::CreateSound(LPCSTR name, BOOL bCtrlFreq, BOOL bNotClip)
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

int	C3DSoundRender::CreateSound(CInifile *pIni, LPCSTR section)
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

void C3DSoundRender::DeleteSound(int& hSound) {
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

C3DSound* C3DSoundRender::GetFreeSound(int hSound) {
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

void C3DSoundRender::Play(int hSound, C3DSound** P, BOOL bLoop, int iLoopCnt)
{
	*P	= GetFreeSound	(hSound);
	(*P)->Play			(P, bLoop, iLoopCnt);
}

void C3DSoundRender::LoadClipPlanes(CInifile *pIni, LPCSTR section) {
	clip_planes.clear();

	CInifile::Sect&	S		= pIni->ReadSection(section);
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
		Fvector				p1,p2,p3;
		FIntersectionQuad	IQ;

		sscanf(I->second, "[%f,%f,%f],[%f,%f,%f],[%f,%f,%f]",
			&p1.x,&p1.y,&p1.z,
			&p2.x,&p2.y,&p2.z,
			&p3.x,&p3.y,&p3.z
			);
		IQ.build(p1,p2,p3);
		clip_planes.push_back(IQ);
	}
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
				sounds[i][0]->Play(sounds[i][0]->owner,sounds[i][0]->bMustLoop, sounds[i][0]->iLoopCountRested);
			}

			// Clones
			for (DWORD j=1; j<sounds[i].size(); j++) {
				sounds[i][j]->Stop();
				_RELEASE(sounds[i][j]->pBuffer3D);
				_RELEASE(sounds[i][j]->pBuffer);
				pSounds->lpDirectSound->DuplicateSoundBuffer(sounds[i][0]->pBuffer,&sounds[i][j]->pBuffer);
				VERIFY	(sounds[i][j]->pBuffer);
				sounds[i][j]->pBuffer->QueryInterface(IID_IDirectSound3DBuffer,(void **)(&sounds[i][j]->pBuffer3D));
				sounds[i][j]->bNeedUpdate = true;
				if (sounds[i][j]->isPlaying()) {
					sounds[i][j]->Play(sounds[i][j]->owner,sounds[i][j]->bMustLoop, sounds[i][j]->iLoopCountRested);
				}
			}
		}
	}
}

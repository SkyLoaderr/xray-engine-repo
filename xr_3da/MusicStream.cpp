// MusicStream.cpp: implementation of the CMusicStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MusicStream.h"
#include "xr_streamsnd.h"

#include "x_ray.h"
#include "xr_smallfont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMusicStream::CMusicStream()
{

}

CMusicStream::~CMusicStream()
{

}

int CMusicStream::FindEmptySlot()
{
	for (DWORD i=0; i<streams.size(); i++) {
		if (streams[i]==0) return i;
	}
	return -1;
}

CSoundStream* CMusicStream::CreateSound	(CInifile *pIni, LPCSTR section)
{
	int slot;
	CSoundStream *pSnd = new CSoundStream;
	pSnd->Load(pIni, section);

	if ((slot=FindEmptySlot())>=0){
		streams[slot] = pSnd;
		return pSnd;
	}

	streams.push_back(pSnd);
	return pSnd;
}

CSoundStream* CMusicStream::CreateSound	(LPCSTR name	)
{
	int slot;
	CSoundStream *pSnd = new CSoundStream;
	pSnd->Load(name);

	if ((slot=FindEmptySlot())>=0){
		streams[slot] = pSnd;
		return pSnd;
	}

	streams.push_back(pSnd);
	return pSnd;
}

void	CMusicStream::DeleteSound	(CSoundStream* pSnd)
{
	int slot;
	for (DWORD i=0; i<streams.size(); i++){
		if (streams[i]==pSnd) { slot = i; break; }
	}

	if (slot>=0){
		_DELETE(streams[slot]);
		pSnd = NULL;
	}
}

void CMusicStream::OnMove()
{
	for(DWORD i=0; i<streams.size(); i++) streams[i]->OnMove();
/*	if (psDeviceFlags&rsStatistic)
	{
		int cnt = 0;
		for(int i=0; i<streams.size(); i++) cnt+=streams[i]->isPlaying()?1:0;
		pApp->pFont->Out(0,0.5f,"%d / %d",cnt,streams.size());
	}
*/
}

void CMusicStream::Reload()
{
	// ...
}

void CMusicStream::Update()
{
	for (DWORD i=0; i<streams.size(); i++)
		if (streams[i]) streams[i]->bNeedUpdate=true;
}

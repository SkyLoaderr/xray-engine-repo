// TextureAVI.cpp: implementation of the CTextureAVI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureAVI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureAVI::CTextureAVI(void *params) : CTexture(params)
{
	pSurface	= NULL;
	pAVI		= NULL;
	Device.seqFrame.Add		(this);
}

CTextureAVI::~CTextureAVI()
{
	Device.seqFrame.Remove	(this);
	VERIFY		(pSurface==NULL);
	_DELETE		(pAVI);
}

void CTextureAVI::Load()
{
	/*
	// make file name
	pAVI	= new CAviPlayerCustom(cName,24);
	pAVI->Play();

	// try to create palettized texture
	DWORD dwF = D3DX_TEXTURE_NOMIPMAP;
	DWORD dwW = pAVI->GetWidth	();
	DWORD dwH = pAVI->GetHeight	();
	D3DX_SURFACEFORMAT fmt = D3DX_SF_R5G5B5;
	pSurface = TUCreateTexture(&dwF, &dwW, &dwH, &fmt, 0, 0);
	if ((dwW != pAVI->GetWidth())||(dwH!=pAVI->GetHeight()))
	{
		Unload();
		Log("! Failed to load AVI texture: ",cName);
	}
	*/
}

void CTextureAVI::Unload()
{
	_DELETE	(pAVI);
	_RELEASE(pSurface);
}

void CTextureAVI::Activate(DWORD dwStage) 
{
	/*
	if (pAVI==0 || pSurface==0) {
		CHK_DX(HW.pDevice->SetTexture(dwStage,0));
	} else {
		if (pAVI->IsUpdated()) {
			TULoadFromBGR24(pSurface,pAVI->GetData());
		}
		CHK_DX(HW.pDevice->SetTexture(dwStage, pSurface));
	}
	*/
}

void CTextureAVI::OnFrame()
{
	/*
	if (pAVI) {
		pAVI->OnMove(Device.fTimeDelta);
		if (!pAVI->IsPlaying()) pAVI->Play();
	}
	*/
}

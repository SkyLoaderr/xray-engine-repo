// TextureLMAP.cpp: implementation of the CTextureLMAP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureLMAP.h"
#include "..\..\ffileops.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureLMAP::CTextureLMAP(void *params) : CTexture(params)
{
	STextureCreation*	P	= (STextureCreation*)params;
	fmt						= D3DFMT_UNKNOWN;
	pSurface				= NULL;
	dwWidth=dwHeight		= D3DX_DEFAULT;
	dwFlags					= 0; //P->bNeedsMipMap?0:1;
}

CTextureLMAP::~CTextureLMAP()
{
	VERIFY(pSurface==NULL);
}

void CTextureLMAP::Unload() {
	_RELEASE	(pSurface);
}

void CTextureLMAP::Activate(DWORD dwStage)
{
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
}

void CTextureLMAP::Load()
{
	pSurface = TWLoader2D(
		cName,
		tpfLM,
		tmDisable, //tmBOX4,
		psTextureLOD,
		psTextureContrast,
		!!(psTextureFlags&1),
		FALSE,

		// return values
		fmt,dwWidth,dwHeight
		);
	
	// misc caps
	bHasAlpha       = TUisAlphaPresents(fmt);
	bDynamic		= false;
}


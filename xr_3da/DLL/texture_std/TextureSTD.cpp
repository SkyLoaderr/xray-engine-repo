// TextureSTD.cpp: implementation of the CTextureSTD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureSTD.h"
#include "..\..\hw.h"
#include "..\..\Texture.h"
#include "..\..\ffileops.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureSTD::CTextureSTD(void *params) : CTexture(params)
{
	STextureCreation*	P	= (STextureCreation*)params;
	fmt						= D3DFMT_UNKNOWN;
	pSurface				= NULL;
	dwWidth=dwHeight		= D3DX_DEFAULT;
	dwFlags					= (P->bNeedsMipMap)?0:1;
}

CTextureSTD::~CTextureSTD()
{
	VERIFY(pSurface==NULL);
}

void CTextureSTD::Activate(DWORD dwStage) {
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
}

void CTextureSTD::Unload()
{
	_RELEASE	(pSurface);
}

void CTextureSTD::Load()
{
static ETexturePF TF2PF[7] = {tpfDefault,tpfDefault,tpf16,tpf32,tpfCompressed,tpfCompressed,tpfCompressed};

	pSurface = TWLoader2D(
		cName,
		TF2PF[psTextureFormat],
		dwFlags?tmDisable:tmBOX4,
		psTextureLOD,
		psTextureContrast,
		!!(psTextureFlags&1),
		false,

		// return values
		fmt,dwWidth,dwHeight
		);
	
	// misc caps
	bHasAlpha       = TUisAlphaPresents(fmt);
	bDynamic		= false;
}


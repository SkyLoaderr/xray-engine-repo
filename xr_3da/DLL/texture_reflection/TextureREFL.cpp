// TextureREFL.cpp: implementation of the CTextureREFL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureREFL.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureREFL::CTextureREFL(void *params) : CTexture(params)
{
	STextureCreation*	P	= (STextureCreation*)params;
	fmt						= D3DFMT_UNKNOWN;
	pSurface				= NULL;
	dwWidth=dwHeight		= D3DX_DEFAULT;
	dwFlags					= 0; //P->bNeedsMipMap?0:D3DX_TEXTURE_NOMIPMAP;
}

CTextureREFL::~CTextureREFL()
{
	VERIFY(pSurface==NULL);
}

void CTextureREFL::Unload() {
	_RELEASE	(pSurface);
}

void CTextureREFL::Activate(DWORD dwStage)
{
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2));
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR ));
	CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+dwStage),
		precalc_identity.d3d()));
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
}

void CTextureREFL::Deactivate(DWORD dwStage)
{
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU+dwStage));
}

void CTextureREFL::Load()
{
	pSurface = TWLoader2D(
		cName,
		tpfLM,
		tmBOX4,
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


// TextureSPENV.cpp: implementation of the CTextureSPENV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureSPENV.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureSPENV::CTextureSPENV(void *params) : CTexture(params)
{
	STextureCreation*	P	= (STextureCreation*)params;
	fmt						= D3DFMT_UNKNOWN;
	pSurface				= NULL;
	dwWidth=dwHeight		= D3DX_DEFAULT;
	dwFlags					= 0; //P->bNeedsMipMap?0:D3DX_TEXTURE_NOMIPMAP;
}

CTextureSPENV::~CTextureSPENV()
{
	VERIFY(pSurface==NULL);
}

void CTextureSPENV::Unload() {
	_RELEASE	(pSurface);
}

/*
// Generate texture coordinates as linear functions 
// such that:
//      u = Ux*x + Uy*y + Uz*z + Uw 
//      v = Vx*x + Vy*y + Vz*z + Vw
// The matrix M for this case is:
//      Ux  Vx  0  0 
//      Uy  Vy  0  0 
//      Uz  Vz  0  0 
//      Uw  Vw  0  0 
*/
void CTextureSPENV::Activate(DWORD dwStage)
{
    // Get the current view matrix
    Fmatrix tex;

	float Ux= .5f*Device.mView._11, Uy= .5f*Device.mView._21, Uz= .5f*Device.mView._31, Uw = .5f;
	float Vx=-.5f*Device.mView._12, Vy=-.5f*Device.mView._22, Vz=-.5f*Device.mView._32, Vw = .5f;

	tex._11=Ux; tex._12=Vx; tex._13=0; tex._14=0;
	tex._21=Uy; tex._22=Vy; tex._23=0; tex._24=0;
	tex._31=Uz; tex._32=Vz; tex._33=0; tex._34=0;
	tex._41=Uw; tex._42=Vw; tex._43=0; tex._44=0;

	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL ));
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2));
	CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+dwStage),
		tex.d3d()));
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
}

/*
	// Old
    // Get the current view matrix
    Fmatrix tex;

	float Ux= .5f*Device.mView._11, Uy= .5f*Device.mView._21, Uz= .5f*Device.mView._31, Uw = .5f;
	float Vx=-.5f*Device.mView._12, Vy=-.5f*Device.mView._22, Vz=-.5f*Device.mView._32, Vw = .5f;

	tex._11=Ux; tex._12=Vx; tex._13=0; tex._14=0;
	tex._21=Uy; tex._22=Vy; tex._23=0; tex._24=0;
	tex._31=Uz; tex._32=Vz; tex._33=0; tex._34=0;
	tex._41=Uw; tex._42=Vw; tex._43=0; tex._44=0;

	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL ));
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2));
	CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATETYPE(D3DTRANSFORMSTATE_TEXTURE0+dwStage),
		tex.d3d()));
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
*/

void CTextureSPENV::Deactivate(DWORD dwStage)
{
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
	CHK_DX(HW.pDevice->SetTextureStageState(
		dwStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU+dwStage));
}

void CTextureSPENV::Load()
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

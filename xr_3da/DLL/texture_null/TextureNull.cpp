// TextureNull.cpp: implementation of the CTextureNull class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureNull.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureNull::CTextureNull( void *params ) : CTexture(params)
{

}

CTextureNull::~CTextureNull()
{

}

void CTextureNull::Activate(DWORD dwStage)
{
	CHK_DX(HW.pDevice->SetTexture(dwStage,0));
}
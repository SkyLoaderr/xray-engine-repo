// TextureSTD.h: interface for the CTextureSTD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURESTD_H__23B78CD8_7AEF_4B48_80CA_338DA3A6CF27__INCLUDED_)
#define AFX_TEXTURESTD_H__23B78CD8_7AEF_4B48_80CA_338DA3A6CF27__INCLUDED_
#pragma once

#include "..\..\texture.h"

class CTextureSTD : public CTexture
{
private:
	D3DFORMAT				fmt;
	DWORD					dwFlags;
	DWORD					dwWidth,dwHeight;
public:
			CTextureSTD		(void *params);
	virtual ~CTextureSTD	();

	virtual void			Load		(void);
	virtual void			Unload		(void);
	virtual void			Activate	(DWORD dwStage);
};

#endif // !defined(AFX_TEXTURESTD_H__23B78CD8_7AEF_4B48_80CA_338DA3A6CF27__INCLUDED_)

// TextureLMAP.h: interface for the CTextureLMAP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURELMAP_H__8388DA24_86AE_4087_847D_C9280D42390C__INCLUDED_)
#define AFX_TEXTURELMAP_H__8388DA24_86AE_4087_847D_C9280D42390C__INCLUDED_
#pragma once

#include "..\..\Texture.h"

class CTextureLMAP : public CTexture  
{
private:
	D3DFORMAT				fmt;
	DWORD					dwFlags;
	DWORD					dwWidth,dwHeight;
public:
			CTextureLMAP	(void *params);
	virtual ~CTextureLMAP	();

	virtual void			Load		(void);
	virtual void			Unload		(void);
	virtual void			Activate	(DWORD dwStage);
};

#endif // !defined(AFX_TEXTURELMAP_H__8388DA24_86AE_4087_847D_C9280D42390C__INCLUDED_)

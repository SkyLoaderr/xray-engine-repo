// TextureFIRE.h: interface for the CTextureFIRE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREFIRE_H__5E1FD7B8_D0C8_41B9_911B_179348D2BF8C__INCLUDED_)
#define AFX_TEXTUREFIRE_H__5E1FD7B8_D0C8_41B9_911B_179348D2BF8C__INCLUDED_
#pragma once

#include "..\..\Texture.h"

class CTextureFIRE : public CTexture  
{
private:
	LPDIRECTDRAWPALETTE		pPalette;
	D3DX_SURFACEFORMAT		fmt;
	WORD					pal2color	[256];
	char*					pEmulSurface;
public:
			CTextureFIRE	(void *params);
	virtual ~CTextureFIRE	();

	virtual void			Load		(void);
	virtual void			Unload		(void);
	virtual void			Activate	(DWORD dwStage);
};

#endif // !defined(AFX_TEXTUREFIRE_H__5E1FD7B8_D0C8_41B9_911B_179348D2BF8C__INCLUDED_)

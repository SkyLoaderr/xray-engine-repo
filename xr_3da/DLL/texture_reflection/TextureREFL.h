// TextureREFL.h: interface for the CTextureREFL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREREFL_H__677B2559_039B_4033_B2D4_59C6262AFF15__INCLUDED_)
#define AFX_TEXTUREREFL_H__677B2559_039B_4033_B2D4_59C6262AFF15__INCLUDED_
#pragma once

#include "..\..\Texture.h"

class CTextureREFL : public CTexture  
{
private:
	D3DFORMAT				fmt;
	DWORD					dwFlags;
	DWORD					dwWidth,dwHeight;
public:
			CTextureREFL	(void *params);
	virtual ~CTextureREFL	();

	virtual void			Load		(void);
	virtual void			Unload		(void);
	virtual void			Activate	(DWORD dwStage);
	virtual void			Deactivate	(DWORD dwStage);
};

#endif // !defined(AFX_TEXTUREREFL_H__677B2559_039B_4033_B2D4_59C6262AFF15__INCLUDED_)

// TextureAVI.h: interface for the CTextureAVI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREAVI_H__B04FE678_5B84_46C7_AB09_93005CB546A0__INCLUDED_)
#define AFX_TEXTUREAVI_H__B04FE678_5B84_46C7_AB09_93005CB546A0__INCLUDED_
#pragma once

#include "..\..\texture.h"
#include "..\..\xr_avi.h"

class CTextureAVI :	public CTexture, public pureFrame
{
	CAviPlayerCustom*		pAVI;
public:
	CTextureAVI				(void *params);
	virtual ~CTextureAVI	();

	virtual void			Load		(void);
	virtual void			Unload		(void);
	virtual void			Activate	(DWORD dwStage);

	virtual void			OnFrame		(void);
};

#endif // !defined(AFX_TEXTUREAVI_H__B04FE678_5B84_46C7_AB09_93005CB546A0__INCLUDED_)

// TextureSPENV.h: interface for the CTextureSPENV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURESPENV_H__7A4222A5_7CFA_4BB6_91B4_4163A3E3DE5A__INCLUDED_)
#define AFX_TEXTURESPENV_H__7A4222A5_7CFA_4BB6_91B4_4163A3E3DE5A__INCLUDED_
#pragma once

#include "..\..\texture.h"

class CTextureSPENV : public CTexture  
{
private:
	D3DFORMAT				fmt;
	DWORD					dwFlags;
	DWORD					dwWidth,dwHeight;
public:
	CTextureSPENV			(void *p);
	virtual ~CTextureSPENV	();

	virtual void			Load		(void);
	virtual void			Unload		(void);
	virtual void			Activate	(DWORD dwStage);
	virtual void			Deactivate	(DWORD dwStage);
};

#endif // !defined(AFX_TEXTURESPENV_H__7A4222A5_7CFA_4BB6_91B4_4163A3E3DE5A__INCLUDED_)

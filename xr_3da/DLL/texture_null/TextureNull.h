// TextureNull.h: interface for the CTextureNull class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURENULL_H__7B99D8D3_69A4_4438_8184_7AE2652CD818__INCLUDED_)
#define AFX_TEXTURENULL_H__7B99D8D3_69A4_4438_8184_7AE2652CD818__INCLUDED_
#pragma once

#include "..\..\texture.h"

class CTextureNull : public CTexture  
{
public:
			CTextureNull	(void *params);
	virtual ~CTextureNull	();

	virtual void			Load		(void)	{};
	virtual void			Unload		(void)	{};
	virtual void			Activate	(DWORD dwStage);
};

#endif // !defined(AFX_TEXTURENULL_H__7B99D8D3_69A4_4438_8184_7AE2652CD818__INCLUDED_)

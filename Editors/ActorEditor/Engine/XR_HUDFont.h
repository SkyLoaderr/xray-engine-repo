#ifndef _XR_HUDFONT_H_
#define _XR_HUDFONT_H_
#pragma once

#include "XR_CustomFont.h"

class ENGINE_API CFontHUD: public CFontBase
{
	float scale;
protected:
	virtual	float	GetRealX(float x)			{return x*scale;}
	virtual	float	GetRealY(float y)			{return y*scale;}
	virtual	float	GetRealWidth(float sz)		{return (sz*TextureSize.x/iNumber)*scale;}
	virtual	float	GetRealHeight(float sz)		{return (sz*TextureSize.y/iNumber)*scale;}
	virtual	float	GetCurrentSize()			{return fCurrentSize*TextureSize.y/iNumber;}
public:
	CFontHUD		();
	virtual	void	OnInitialize	();

	void			SetScale		(float sc)	{scale=sc;		}
	float			GetScale		()			{return scale;	}
};

#endif // _XR_HUDFONT_H_

#ifndef _XR_HUDFONT_H_
#define _XR_HUDFONT_H_
#pragma once

#include "XR_CustomFont.h"

class ENGINE_API CFontHUD: public CFontBase
{
	float scale;
protected:
	IC virtual	float		GetRealX(float x)		{return x*scale;}
	IC virtual	float		GetRealY(float y)		{return y*scale;}
	IC virtual	float		GetRealWidth(float sz)	{return (sz*TextureSize.x/iNumber)*scale;}
	IC virtual	float		GetRealHeight(float sz)	{return (sz*TextureSize.y/iNumber)*scale;}
	IC virtual	float		GetCurrentSize()		{return fCurrentSize*TextureSize.y/iNumber;}
public:
					CFontHUD	();
	void			SetScale	(float sc){scale=sc;}
	float			GetScale	(){return scale;}
};

#endif // _XR_HUDFONT_H_

#include "stdafx.h"

void	CRenderTarget::phase_combine	()
{
	// 
	dwWidth						= Device.dwWidth;
	dwHeight					= Device.dwHeight;
	RCache.set_RT				(HW.pBaseRT,			0);
	RCache.set_RT				(NULL,					1);
	RCache.set_RT				(NULL,					2);
	RCache.set_ZB				(HW.pBaseZB);
	RImplementation.rmNormal	();

	// Draw full-screen quad textured with our scene image
	u32	Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);

	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Fill vertex buffer
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPS,			float(_h+EPS),	EPS,	1.f, C, p0.x, p1.y);	pv++;
	pv->set						(EPS,			EPS,			EPS,	1.f, C, p0.x, p0.y);	pv++;
	pv->set						(float(_w+EPS),	float(_h+EPS),	EPS,	1.f, C, p1.x, p1.y);	pv++;
	pv->set						(float(_w+EPS),	EPS,			EPS,	1.f, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Shader			(s_combine_dbg_Color);
	RCache.set_Geometry			(g_combine);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

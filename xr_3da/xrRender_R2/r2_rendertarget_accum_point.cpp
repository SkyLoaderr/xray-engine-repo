#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	u32							Offset;
	Fvector2					p0,p1;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Calc near plane of light
	float	d_Z	= EPS_S, d_W = 1.f;

	// Fill vertex buffer
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);
	RCache.set_Geometry			(g_combine);
	RCache.set_Shader			(s_accum_point);

	// Constants
	Fvector		L_pos;
	Fcolor		L_clr			= L->color;
	Device.mView.transform_dir	(L_pos,L->sphere.P);
	RCache.set_c				("light_position",	L_pos.x,L_pos.y,L_pos.z,0.f);
	RCache.set_c				("light_color",		L_clr.r,L_clr.g,L_clr.b,.15f*L_clr.magnitude_rgb());

	// Render if stencil >= 0x2
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

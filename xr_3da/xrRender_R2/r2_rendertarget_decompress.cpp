#include "stdafx.h"

void	CRenderTarget::phase_decompress		()
{
	// Common
	CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,D3DCULL_NONE)); 	
	RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);

	Fvector2					p0,p1;
	u32		Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	float	d_Z					= EPS_S;
	float	d_W					= 1.f;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Fill vertex buffer
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);
	RCache.set_Geometry			(g_combine);

	// Position
	u_setrt						(rt_Position,NULL,NULL,HW.pBaseZB);
	RCache.set_Element			(s_decompress->E[0]);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

	// Normal
	u_setrt						(rt_Normal,NULL,NULL,HW.pBaseZB);
	RCache.set_Element			(s_decompress->E[1]);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

	// Color
	u_setrt						(rt_Color,NULL,NULL,HW.pBaseZB);
	RCache.set_Element			(s_decompress->E[2]);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

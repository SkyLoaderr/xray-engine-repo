#include "stdafx.h"

void	CRenderTarget::phase_decompress		()
{
	// Common
	RCache.set_CullMode			(CULL_NONE);
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

	if (RImplementation.b_nv3x)
	{
		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_decompress->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_decompress->vb_stride);
		RCache.set_Geometry			(g_decompress);

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
	} else {
		struct v_dc		{
			Fvector4	p;
			Fvector2	uv0;
			Fvector4	uv1;
			void		set			(float x, float y, float u0, float v0, float u1, float v1)	{
				p.set	(x,y,EPS_S,1);
				uv0.set	(u0,v0);
				uv1.set	(u1,v1,0,1);
			}
		};
		VERIFY						(g_decompress->vb_stride == sizeof(v_dc));

		// Fill vertex buffer
		v_dc* pv					= (v_dc*) RCache.Vertex.Lock	(4,g_decompress->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	p0.x, p1.y, -1, -1);	pv++;
		pv->set						(EPS,			EPS,			p0.x, p0.y, -1, +1);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	p1.x, p1.y, +1, -1);	pv++;
		pv->set						(float(_w+EPS),	EPS,			p1.x, p0.y, +1, +1);	pv++;
		RCache.Vertex.Unlock		(4,g_decompress->vb_stride);
		RCache.set_Geometry			(g_decompress);

		// Decompress
		Fmatrix		mUnwarp;
		D3DXMatrixInverse			((D3DXMATRIX*)&mUnwarp,0,(D3DXMATRIX*)&Device.mProject);
		// mUnwarp.invert			(Device.mProject);
		u_setrt						(rt_Position,rt_Normal,NULL,HW.pBaseZB);
		RCache.set_Shader			(s_decompress);
		RCache.set_c				("m_unwarp",mUnwarp);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}

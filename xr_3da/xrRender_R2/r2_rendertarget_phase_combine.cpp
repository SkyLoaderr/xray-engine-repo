#include "stdafx.h"

void	CRenderTarget::phase_combine	()
{
	// Targets
	u_setrt								(Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	u32			Offset					= 0;
	Fvector2	p0,p1;

	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	
	RCache.set_Stencil					(FALSE);

	// Draw full-screen quad textured with our SKYBOX
	{
		u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
		float	_w					= float			(Device.dwWidth);
		float	_h					= float			(Device.dwHeight);

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
		RCache.set_Element			(s_combine->E[0]);
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil	(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);

	// Draw full-screen quad textured with our scene image
	{
		u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
		float	_w					= float(Device.dwWidth);
		float	_h					= float(Device.dwHeight);

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
		float dr					= ps_r2_ls_dynamic_range;
		RCache.set_Element			(s_combine->E[1]);
		RCache.set_c				("light_dynamic_range",	dr,dr,dr,dr);
		RCache.set_c				("light_hemi",			.5f,.5f,.5f,0.f);
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	//
	RCache.set_Stencil				(FALSE);

	// ********************* Debug
	if (1)
	{
		u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
		float	_w					= float(Device.dwWidth)/2;
		float	_h					= float(Device.dwHeight)/2;

		// Draw quater-screen quad textured with our direct-shadow-map-image
		{
			u32							IX=0,IY=1;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

			// Fill vertex buffer
			FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
			pv->set						((IX+0)*_w+EPS,	(IY+1)*_h+EPS,	EPS,	1.f, C, p0.x, p1.y);	pv++;
			pv->set						((IX+0)*_w+EPS,	(IY+0)*_h+EPS,	EPS,	1.f, C, p0.x, p0.y);	pv++;
			pv->set						((IX+1)*_w+EPS,	(IY+1)*_h+EPS,	EPS,	1.f, C, p1.x, p1.y);	pv++;
			pv->set						((IX+1)*_w+EPS,	(IY+0)*_h+EPS,	EPS,	1.f, C, p1.x, p0.y);	pv++;
			RCache.Vertex.Unlock		(4,g_combine->vb_stride);

			// Draw COLOR
			RCache.set_Shader			(s_bloom_dbg_1);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
		// Draw quater-screen quad textured with our accumulator
		{
			u32							IX=1,IY=1;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

			// Fill vertex buffer
			FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
			pv->set						((IX+0)*_w+EPS,	(IY+1)*_h+EPS,	EPS,	1.f, C, p0.x, p1.y);	pv++;
			pv->set						((IX+0)*_w+EPS,	(IY+0)*_h+EPS,	EPS,	1.f, C, p0.x, p0.y);	pv++;
			pv->set						((IX+1)*_w+EPS,	(IY+1)*_h+EPS,	EPS,	1.f, C, p1.x, p1.y);	pv++;
			pv->set						((IX+1)*_w+EPS,	(IY+0)*_h+EPS,	EPS,	1.f, C, p1.x, p0.y);	pv++;
			RCache.Vertex.Unlock		(4,g_combine->vb_stride);

			// Draw COLOR
			RCache.set_Shader			(s_combine_dbg_DepthD);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}
}

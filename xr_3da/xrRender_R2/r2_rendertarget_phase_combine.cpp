#include "stdafx.h"

void	CRenderTarget::phase_combine	()
{
	// Targets
	dwWidth								= Device.dwWidth;
	dwHeight							= Device.dwHeight;
	RCache.set_RT						(HW.pBaseRT,			0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(HW.pBaseZB);
	RImplementation.rmNormal			();

	// Clear	- don't clear - it's stupid here :)

	// Stencil	- disable
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		FALSE				));

	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	

	// Draw full-screen quad textured with our scene image
	{
		u32		Offset;
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
		float dr					= ps_r2_ls_dynamic_range;
		RCache.set_Shader			(s_combine);
		RCache.set_c				("light_dynamic_range",	dr,dr,dr,dr);
		RCache.set_c				("light_hemi",			dr,dr,dr,dr);
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// ********************* Debug
	if (0)
	{
		u32		Offset;
		u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
		float	_w					= float(Device.dwWidth)/4;
		float	_h					= float(Device.dwHeight)/4;

		// Draw quater-screen quad textured with our direct-shadow-map-image
		/*
		{
			u32							IX=0,IY=0;
			Fvector2					p0,p1;
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
			RCache.set_Shader			(s_smap_d_debug);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
		*/
		// Draw quater-screen quad textured with our accumulator
		{
			u32							IX=1,IY=0;
			Fvector2					p0,p1;
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
			RCache.set_Shader			(s_combine_dbg_Accumulator);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}
}

#include "stdafx.h"

void	CRenderTarget::phase_combine	()
{
	// Targets
	u_setrt								(Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	u32			Offset					= 0;
	Fvector2	p0,p1;

	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	
	RCache.set_Stencil					( FALSE );

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
	if (0)	// combine
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
		RCache.set_c				("light_hemi",			ps_r2_ls_hemi.x,ps_r2_ls_hemi.y,ps_r2_ls_hemi.z,0.f);
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	if (1)	// AA
	{
		struct v_aa	{
			Fvector4	p;
			Fvector2	uv0;
			Fvector2	uv1;
			Fvector2	uv2;
			Fvector2	uv3;
			Fvector2	uv4;
		};

		float	_w					= float(Device.dwWidth);
		float	_h					= float(Device.dwHeight);
		float	ddw					= 1.f/_w;
		float	ddh					= 1.f/_h;
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill vertex buffer
		v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw,p1.y);pv->uv2.set(p0.x+ddw,p1.y);pv->uv3.set(p0.x,p1.y-ddh);pv->uv4.set(p0.x,p1.y+ddh);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw,p0.y);pv->uv2.set(p0.x+ddw,p0.y);pv->uv3.set(p0.x,p0.y-ddh);pv->uv4.set(p0.x,p0.y+ddh);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw,p1.y);pv->uv2.set(p1.x+ddw,p1.y);pv->uv3.set(p1.x,p1.y-ddh);pv->uv4.set(p1.x,p1.y+ddh);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw,p0.y);pv->uv2.set(p1.x+ddw,p0.y);pv->uv3.set(p1.x,p0.y-ddh);pv->uv4.set(p1.x,p0.y+ddh);pv++;
		RCache.Vertex.Unlock		(4,g_aa->vb_stride);

		// Draw COLOR
		float dr					= ps_r2_ls_dynamic_range;
		RCache.set_Element			(s_combine->E[2]);
		RCache.set_c				("e_barrier",	ps_r2_aa_barier.x, ps_r2_aa_barier.y, ps_r2_aa_barier.z, 0);
		RCache.set_c				("e_weights",	ps_r2_aa_weight.x, ps_r2_aa_weight.y, ps_r2_aa_weight.z, 0);
		RCache.set_Geometry			(g_aa);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	//
	RCache.set_Stencil				(FALSE);

	// ********************* Debug
	if (0)
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

#include "stdafx.h"

void	CRenderTarget::phase_combine	()
{
	u32			Offset					= 0;
	Fvector2	p0,p1;
	/*
	if (!ps_r2_ls_flags.test(R2FLAG_AA))
	{
	*/
		u_setrt								( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
		RCache.set_CullMode					( CULL_NONE );
		RCache.set_Stencil					( FALSE );

		// Draw full-screen quad textured with our SKYBOX
		/*
		if (0)
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
		*/

		// Stencil	- draw only where stencil >= 0x1
		// RCache.set_Stencil	(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
		RCache.set_Stencil				( FALSE );

		// Draw full-screen quad textured with our scene image
		if (1)	// combine
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
			RCache.set_c				("L_hdr",	dr,dr,dr,dr);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	/*
	} else {
		// Targets
		u_setrt								(rt_Generic,NULL,NULL,HW.pBaseZB);
		// Misc		- draw everything (no culling)
		RCache.set_CullMode					( CULL_NONE );
		RCache.set_Stencil					( FALSE );

		// Draw full-screen quad textured with our SKYBOX
		if (1)
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
		if (1)	// combine
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
			RCache.set_c				("L_hdr",	dr,dr,dr,dr);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}

		// AA-blur
		u_setrt							(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Stencil				(FALSE);
		if (1)	
		{
			struct v_aa	{
				Fvector4	p;
				Fvector2	uv0;
				Fvector2	uv1;
				Fvector2	uv2;
				Fvector2	uv3;
			};

			float	_w					= float(Device.dwWidth);
			float	_h					= float(Device.dwHeight);
			float	kw					= (1.f/_w)*ps_r2_aa_kernel;
			float	kh					= (1.f/_h)*ps_r2_aa_kernel;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

			// Fill vertex buffer
			v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa_blur->vb_stride,Offset);
			pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x-kw,p1.y-kh);pv->uv1.set(p0.x+kw,p1.y+kh);pv->uv2.set(p0.x+kw,p1.y-kh);pv->uv3.set(p0.x-kw,p1.y+kh);pv++;
			pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x-kw,p0.y-kh);pv->uv1.set(p0.x+kw,p0.y+kh);pv->uv2.set(p0.x+kw,p0.y-kh);pv->uv3.set(p0.x-kw,p0.y+kh);pv++;
			pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x-kw,p1.y-kh);pv->uv1.set(p1.x+kw,p1.y+kh);pv->uv2.set(p1.x+kw,p1.y-kh);pv->uv3.set(p1.x-kw,p1.y+kh);pv++;
			pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x-kw,p0.y-kh);pv->uv1.set(p1.x+kw,p0.y+kh);pv->uv2.set(p1.x+kw,p0.y-kh);pv->uv3.set(p1.x-kw,p0.y+kh);pv++;
			RCache.Vertex.Unlock		(4,g_aa_blur->vb_stride);

			// Draw COLOR
			RCache.set_Element			(s_combine->E[2]);
			RCache.set_Geometry			(g_aa_blur);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}

		// AA
		u_setrt							(Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
		RCache.set_Stencil				(FALSE);
		if (1)	
		{
			struct v_aa	{
				Fvector4	p;
				Fvector2	uv0;
				Fvector2	uv1;
				Fvector2	uv2;
				Fvector2	uv3;
				Fvector2	uv4;
				Fvector4	uv5;
				Fvector4	uv6;
			};

			float	_w					= float(Device.dwWidth);
			float	_h					= float(Device.dwHeight);
			float	ddw					= 1.f/_w;
			float	ddh					= 1.f/_h;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

			// Fill vertex buffer
			v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa_AA->vb_stride,Offset);
			pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw,p1.y-ddh);pv->uv2.set(p0.x+ddw,p1.y+ddh);pv->uv3.set(p0.x+ddw,p1.y-ddh);pv->uv4.set(p0.x-ddw,p1.y+ddh);pv->uv5.set(p0.x-ddw,p1.y,p1.y,p0.x+ddw);pv->uv6.set(p0.x,p1.y-ddh,p1.y+ddh,p0.x);pv++;
			pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw,p0.y-ddh);pv->uv2.set(p0.x+ddw,p0.y+ddh);pv->uv3.set(p0.x+ddw,p0.y-ddh);pv->uv4.set(p0.x-ddw,p0.y+ddh);pv->uv5.set(p0.x-ddw,p0.y,p0.y,p0.x+ddw);pv->uv6.set(p0.x,p0.y-ddh,p0.y+ddh,p0.x);pv++;
			pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw,p1.y-ddh);pv->uv2.set(p1.x+ddw,p1.y+ddh);pv->uv3.set(p1.x+ddw,p1.y-ddh);pv->uv4.set(p1.x-ddw,p1.y+ddh);pv->uv5.set(p1.x-ddw,p1.y,p1.y,p1.x+ddw);pv->uv6.set(p1.x,p1.y-ddh,p1.y+ddh,p1.x);pv++;
			pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw,p0.y-ddh);pv->uv2.set(p1.x+ddw,p0.y+ddh);pv->uv3.set(p1.x+ddw,p0.y-ddh);pv->uv4.set(p1.x-ddw,p0.y+ddh);pv->uv5.set(p1.x-ddw,p0.y,p0.y,p1.x+ddw);pv->uv6.set(p1.x,p0.y-ddh,p0.y+ddh,p1.x);pv++;
			RCache.Vertex.Unlock		(4,g_aa_AA->vb_stride);

			// Draw COLOR
			RCache.set_Element			(s_combine->E[3]);
			RCache.set_c				("e_barrier",	ps_r2_aa_barier.x, ps_r2_aa_barier.y, ps_r2_aa_barier.z, 0);
			RCache.set_c				("e_weights",	ps_r2_aa_weight.x, ps_r2_aa_weight.y, ps_r2_aa_weight.z, 0);
			RCache.set_Geometry			(g_aa_AA);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}
	*/

	//
	RCache.set_Stencil				(FALSE);

	// ********************* Debug
	if (1)
	{
		u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
		float	_w					= float(Device.dwWidth)/2;
		float	_h					= float(Device.dwHeight)/2;

		// Draw quater-screen quad textured with our direct-shadow-map-image
		if (0) //.
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
			RCache.set_Shader			(s_combine_dbg_Normal);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
		// Draw quater-screen quad textured with our accumulator
		if (0)
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

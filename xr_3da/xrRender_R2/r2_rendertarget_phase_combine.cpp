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
	u32			Offset					= 0;
	Fvector2	p0,p1;

	// Clear	- don't clear - it's stupid here :)

	// Stencil	- draw only where stencil >= 0x1
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x01				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0x00				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));

	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	

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
		RCache.set_Element			(s_combine->E[0]);
		RCache.set_c				("light_dynamic_range",	dr,dr,dr,dr);
		RCache.set_c				("light_hemi",			.5f,.5f,.5f,0.f);
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Prepare skybox rendering (only, where stencil==0)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_EQUAL		));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x00				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0x00				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));	
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));

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
		RCache.set_Element			(s_combine->E[1]);
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	//
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		FALSE				));

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
			RCache.set_Shader			(s_bloom_dbg_2);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}
}

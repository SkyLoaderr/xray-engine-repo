#include "stdafx.h"

struct v_build	{
	Fvector4	p;
	Fvector2	uv0;
	Fvector2	uv1;
	Fvector2	uv2;
	Fvector2	uv3;
};

struct v_filter {
	Fvector4	p;
	Fvector4	uv0;
	Fvector4	uv1;
	Fvector4	uv2;
	Fvector4	uv3;
	Fvector4	uv4;
	Fvector4	uv5;
	Fvector4	uv6;
	Fvector4	uv7;
};

void CRenderTarget::phase_bloom	()
{
	u32		Offset;

	// Targets
	dwWidth								= Device.dwWidth	/2;
	dwHeight							= Device.dwHeight	/2;
	RCache.set_RT						(rt_Bloom_1->pRT,		0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(NULL);					// No need for ZBuffer at all
	RImplementation.rmNormal			();

	// Clear	- don't clear - it's stupid here :)
	// Stencil	- disable
	// Misc		- draw everything (no culling)
	CHK_DX	(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		FALSE				));
	CHK_DX	(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	

	// Transfer into Bloom1
	float		_w				= float(Device.dwWidth);
	float		_h				= float(Device.dwHeight);
	float		_2w				= _w/2;
	float		_2h				= _h/2;
	Fvector2	one				= { 1.f/_w, 1.f/_h };
	Fvector2	half			= { .5f/_w, .5f/_h };
	Fvector2	a_0				= { half.x + 0,		half.y + 0		};
	Fvector2	a_1				= { half.x + one.x, half.y + 0		};
	Fvector2	a_2				= { half.x + 0,		half.y + one.y	};
	Fvector2	a_3				= { half.x + one.x,	half.y + one.y	};
	Fvector2	b_0				= { 1 + a_0.x,		1 + a_0.y		};
	Fvector2	b_1				= { 1 + a_1.x,		1 + a_1.y		};
	Fvector2	b_2				= { 1 + a_2.x,		1 + a_2.y		};
	Fvector2	b_3				= { 1 + a_3.x,		1 + a_3.y		};

	// Fill vertex buffer
	v_build* pv					= (v_build*) RCache.Vertex.Lock	(4,g_bloom_build->vb_stride,Offset);
	pv->p.set	(EPS,			float(_2h+EPS),	EPS,1.f);	
	pv->uv0.set	(a_0.x,b_0.y);
	pv->uv1.set	(a_1.x,b_1.y);
	pv->uv2.set	(a_2.x,b_2.y);
	pv->uv3.set	(a_3.x,b_3.y);
	pv++;

	pv->p.set	(EPS,			EPS,			EPS,1.f);	
	pv->uv0.set	(a_0.x,a_0.y);
	pv->uv1.set	(a_1.x,a_1.y);
	pv->uv2.set	(a_2.x,a_2.y);
	pv->uv3.set	(a_3.x,a_3.y);
	pv++;

	pv->p.set	(float(_2w+EPS),float(_2h+EPS),	EPS,1.f);	
	pv->uv0.set	(b_0.x,b_0.y);
	pv->uv1.set	(b_1.x,b_1.y);
	pv->uv2.set	(b_2.x,b_2.y);
	pv->uv3.set	(b_3.x,b_3.y);
	pv++;

	pv->p.set	(float(_2w+EPS),EPS,			EPS,1.f);	
	pv->uv0.set	(b_0.x,a_0.y);
	pv->uv1.set	(b_1.x,a_1.y);
	pv->uv2.set	(b_2.x,a_2.y);
	pv->uv3.set	(b_3.x,a_3.y);
	pv++;

	RCache.Vertex.Unlock		(4,g_bloom_build->vb_stride);

	// Perform combine
	float dr					= ps_r2_ls_dynamic_range;
	RCache.set_Element			(s_bloom->E[0]);
	RCache.set_c				("light_dynamic_range",	dr,dr,dr,dr);
	RCache.set_c				("light_hemi",			.5f,.5f,.5f,0.f);
	RCache.set_Geometry			(g_bloom_build);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

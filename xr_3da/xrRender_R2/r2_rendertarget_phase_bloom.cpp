#include "stdafx.h"

#pragma pack(push,4)
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
#pragma pack(pop)

// Gauss filtering coeffs
// Samples:			0-central, -1, -2,..., -7, 1, 2,... 7
// 
void	CalcGauss_k7(
					 Fvector4&	w0,					// weight
					 Fvector4&	w1,					// weight
					 float		r		=3.3f,		// gaussian radius
					 float		s_out	=1.f		// resulting magnitude
					 )
{
	float				W[8];

	// calculate
	float mag					=	0;
	for (int i=-7; i<=0; i++)	W[-i]	=	expf	(-float(i*i)/(2*r*r));	// weight
	for (i=0; i<8; i++)	mag		+= i?2*W[i]:W[i];							// symmetrical weight
	for (i=0; i<8; i++)	W[i]	= s_out*W[i]/mag;

	// W[0]=0, W[7]=-7
	w0.set	(W[1],W[2],W[3],W[4]);		// -1, -2, -3, -4
	w1.set	(W[5],W[6],W[7],W[0]);		// -5, -6, -7, 0
}

void CRenderTarget::phase_bloom	()
{
	u32		Offset;

	// Targets
	RCache.set_RT						(rt_Bloom_1->pRT,		0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(NULL);					// No need for ZBuffer at all
	
	// XForms
	RCache.set_xform_world				(Fidentity);
	RCache.set_xform_view				(Device.mView);
	RCache.set_xform_project			(Device.mProject);

	// Clear	- don't clear - it's stupid here :)
	// Stencil	- disable
	// Misc		- draw everything (no culling)
	CHK_DX	(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		FALSE				));
	CHK_DX	(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	

	// Render skybox/skydome into Bloom1
	RImplementation.rmFar				();
	pCreator->Environment.RenderFirst	();		// sky
	RImplementation.rmNormal			();

	// Transfer into Bloom1
	{
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
		pv->uv0.set	(a_0.x,b_0.y);	pv->uv1.set	(a_1.x,b_1.y);	pv->uv2.set	(a_2.x,b_2.y);	pv->uv3.set	(a_3.x,b_3.y);
		pv++;
		pv->p.set	(EPS,			EPS,			EPS,1.f);	
		pv->uv0.set	(a_0.x,a_0.y);	pv->uv1.set	(a_1.x,a_1.y);	pv->uv2.set	(a_2.x,a_2.y);	pv->uv3.set	(a_3.x,a_3.y);
		pv++;
		pv->p.set	(float(_2w+EPS),float(_2h+EPS),	EPS,1.f);	
		pv->uv0.set	(b_0.x,b_0.y);	pv->uv1.set	(b_1.x,b_1.y);	pv->uv2.set	(b_2.x,b_2.y);	pv->uv3.set	(b_3.x,b_3.y);
		pv++;
		pv->p.set	(float(_2w+EPS),EPS,			EPS,1.f);	
		pv->uv0.set	(b_0.x,a_0.y);	pv->uv1.set	(b_1.x,a_1.y);	pv->uv2.set	(b_2.x,a_2.y);	pv->uv3.set	(b_3.x,a_3.y);
		pv++;
		RCache.Vertex.Unlock		(4,g_bloom_build->vb_stride);

		// Perform combine (all scalers must account for 4 samples + final diffuse multiply);
		float lscale				= ps_r2_ls_bloom_threshold;		// must be .25 infact
		float ldr					= .25f*ps_r2_ls_dynamic_range;	ldr *= lscale;
		float lh					= .25f*.5f;						lh	*= lscale;
		RCache.set_Element			(s_bloom->E[0]);
		RCache.set_c				("light_dynamic_range",	ldr,ldr,ldr,ldr);
		RCache.set_c				("light_hemi",			lh,lh,lh,0.f);
		RCache.set_Geometry			(g_bloom_build);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Transfer into Bloom2, horizontal filter
	{
		float		_w				= float	(Device.dwWidth		/ 2);
		float		_h				= float	(Device.dwHeight	/ 2);
		Fvector2	two				= { 2.f/_w, 2.f/_h };
		Fvector2	one				= { 1.f/_w, 1.f/_h };
		Fvector2	half			= { .5f/_w, .5f/_h };
		Fvector4	a_0				= { half.x,					half.y,	half.y,		half.x					};	// center
		Fvector4	a_1				= { a_0.x - one.x - half.x, half.y,	half.y,		a_0.w + one.x + half.x	};	// -1,+1i
		Fvector4	a_2				= { a_1.x - two.x,			half.y,	half.y,		a_1.w + two.x			};	// -2,+2i
		Fvector4	a_3				= { a_2.x - two.x,			half.y,	half.y,		a_2.w + two.x			};	// -3,+3i
		Fvector4	a_4				= { a_3.x - two.x,			half.y,	half.y,		a_3.w + two.x			};	// -4,+4i
		Fvector4	a_5				= { a_4.x - two.x,			half.y,	half.y,		a_4.w + two.x			};	// -5,+5i
		Fvector4	a_6				= { a_5.x - two.x,			half.y,	half.y,		a_5.w + two.x			};	// -6,+6i
		Fvector4	a_7				= { a_6.x - two.x,			half.y,	half.y,		a_6.w + two.x			};	// -7,+7i

		// Fill vertex buffer
		v_filter* pv				= (v_filter*) RCache.Vertex.Lock	(4,g_bloom_filter->vb_stride,Offset);

		// 0 - LB
		pv->p.set	(EPS,			float(_h+EPS),	EPS,1.f);	
		pv->uv0.set	(a_0.x,1+a_0.y,0,0);	
		pv->uv1.set	(a_1.x,1+a_1.y,1+a_1.z,a_1.w);	
		pv->uv2.set	(a_2.x,1+a_2.y,1+a_2.z,a_2.w);	
		pv->uv3.set	(a_3.x,1+a_3.y,1+a_3.z,a_3.w);
		pv->uv4.set	(a_4.x,1+a_4.y,1+a_4.z,a_4.w);
		pv->uv5.set	(a_5.x,1+a_5.y,1+a_5.z,a_5.w);
		pv->uv6.set	(a_6.x,1+a_6.y,1+a_6.z,a_6.w);
		pv->uv7.set	(a_7.x,1+a_7.y,1+a_7.z,a_7.w);
		pv++;

		// 1 - LT
		pv->p.set	(EPS,			EPS,			EPS,1.f);	
		pv->uv0.set	(a_0.x,a_0.y,0,0);	
		pv->uv1.set	(a_1.x,a_1.y,a_1.z,a_1.w);
		pv->uv2.set	(a_2.x,a_2.y,a_2.z,a_2.w);
		pv->uv3.set	(a_3.x,a_3.y,a_3.z,a_3.w);
		pv->uv4.set	(a_4.x,a_4.y,a_4.z,a_4.w);
		pv->uv5.set	(a_5.x,a_5.y,a_5.z,a_5.w);
		pv->uv6.set	(a_6.x,a_6.y,a_6.z,a_6.w);
		pv->uv7.set	(a_7.x,a_7.y,a_7.z,a_7.w);
		pv++;

		// 2 - RB
		pv->p.set	(float(_w+EPS),	float(_h+EPS),	EPS,1.f);	
		pv->uv0.set	(1+a_0.x,1+a_0.y,0,0);	
		pv->uv1.set	(1+a_1.x,1+a_1.y,1+a_1.z,1+a_1.w);	
		pv->uv2.set	(1+a_2.x,1+a_2.y,1+a_2.z,1+a_2.w);	
		pv->uv3.set	(1+a_3.x,1+a_3.y,1+a_3.z,1+a_3.w);
		pv->uv4.set	(1+a_4.x,1+a_4.y,1+a_4.z,1+a_4.w);
		pv->uv5.set	(1+a_5.x,1+a_5.y,1+a_5.z,1+a_5.w);
		pv->uv6.set	(1+a_6.x,1+a_6.y,1+a_6.z,1+a_6.w);
		pv->uv7.set	(1+a_7.x,1+a_7.y,1+a_7.z,1+a_7.w);
		pv++;								

		// 3 - RT
		pv->p.set	(float(_w+EPS),	EPS,			EPS,1.f);	
		pv->uv0.set	(1+a_0.x,a_0.y,0,0);	
		pv->uv1.set	(1+a_1.x,a_1.y,a_1.z,1+a_1.w);	
		pv->uv2.set	(1+a_2.x,a_2.y,a_2.z,1+a_2.w);	
		pv->uv3.set	(1+a_3.x,a_3.y,a_3.z,1+a_3.w);
		pv->uv4.set	(1+a_4.x,a_4.y,a_4.z,1+a_4.w);
		pv->uv5.set	(1+a_5.x,a_5.y,a_5.z,1+a_5.w);
		pv->uv6.set	(1+a_6.x,a_6.y,a_6.z,1+a_6.w);
		pv->uv7.set	(1+a_7.x,a_7.y,a_7.z,1+a_7.w);
		pv++;							 
		RCache.Vertex.Unlock		(4,g_bloom_filter->vb_stride);

		// Perform filtering
		Fvector4	w0,w1;
		CalcGauss_k7				(w0,w1,ps_r2_ls_bloom_kernel,1.f);
		RCache.set_RT				(rt_Bloom_2->pRT,		0);
		RCache.set_Element			(s_bloom->E[1]);
		RCache.set_ca				("weight", 0,			w0);
		RCache.set_ca				("weight", 1,			w1);
		RCache.set_Geometry			(g_bloom_filter);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Transfer into Bloom1, vertical filter
	{
		float		_w				= float	(Device.dwWidth		/ 2);
		float		_h				= float	(Device.dwHeight	/ 2);
		Fvector2	two				= { 2.f/_w, 2.f/_h };
		Fvector2	one				= { 1.f/_w, 1.f/_h };
		Fvector2	half			= { .5f/_w, .5f/_h };
		Fvector4	a_0				= { half.x,	half.y,					half.y,						half.x	};	// center
		Fvector4	a_1				= { half.x, a_0.y - one.y - half.y,	half.y + one.y + a_0.z,		half.x	};	// -1,+1i
		Fvector4	a_2				= { half.x,			a_1.y - two.y,	two.y + a_1.z,				half.x	};	// -2,+2i
		Fvector4	a_3				= { half.x,			a_2.y - two.y,	two.y + a_2.z,				half.x	};	// -3,+3i
		Fvector4	a_4				= { half.x,			a_3.y - two.y,	two.y + a_3.z,				half.x	};	// -4,+4i
		Fvector4	a_5				= { half.x,			a_4.y -	two.y,	two.y + a_4.z,				half.x	};	// -5,+5i
		Fvector4	a_6				= { half.x,			a_5.y -	two.y,	two.y + a_5.z,				half.x	};	// -6,+6i
		Fvector4	a_7				= { half.x,			a_6.y - two.y,	two.y + a_6.z,				half.x	};	// -7,+7i

		// Fill vertex buffer
		v_filter* pv				= (v_filter*) RCache.Vertex.Lock	(4,g_bloom_filter->vb_stride,Offset);

		// 0 - LB
		pv->p.set	(EPS,			float(_h+EPS),	EPS,1.f);	
		pv->uv0.set	(a_0.x,1+a_0.y,0,0);	
		pv->uv1.set	(a_1.x,1+a_1.y,1+a_1.z,a_1.w);	
		pv->uv2.set	(a_2.x,1+a_2.y,1+a_2.z,a_2.w);	
		pv->uv3.set	(a_3.x,1+a_3.y,1+a_3.z,a_3.w);
		pv->uv4.set	(a_4.x,1+a_4.y,1+a_4.z,a_4.w);
		pv->uv5.set	(a_5.x,1+a_5.y,1+a_5.z,a_5.w);
		pv->uv6.set	(a_6.x,1+a_6.y,1+a_6.z,a_6.w);
		pv->uv7.set	(a_7.x,1+a_7.y,1+a_7.z,a_7.w);
		pv++;

		// 1 - LT
		pv->p.set	(EPS,			EPS,			EPS,1.f);	
		pv->uv0.set	(a_0.x,a_0.y,0,0);	
		pv->uv1.set	(a_1.x,a_1.y,a_1.z,a_1.w);
		pv->uv2.set	(a_2.x,a_2.y,a_2.z,a_2.w);
		pv->uv3.set	(a_3.x,a_3.y,a_3.z,a_3.w);
		pv->uv4.set	(a_4.x,a_4.y,a_4.z,a_4.w);
		pv->uv5.set	(a_5.x,a_5.y,a_5.z,a_5.w);
		pv->uv6.set	(a_6.x,a_6.y,a_6.z,a_6.w);
		pv->uv7.set	(a_7.x,a_7.y,a_7.z,a_7.w);
		pv++;

		// 2 - RB
		pv->p.set	(float(_w+EPS),	float(_h+EPS),	EPS,1.f);	
		pv->uv0.set	(1+a_0.x,1+a_0.y,0,0);	
		pv->uv1.set	(1+a_1.x,1+a_1.y,1+a_1.z,1+a_1.w);	
		pv->uv2.set	(1+a_2.x,1+a_2.y,1+a_2.z,1+a_2.w);	
		pv->uv3.set	(1+a_3.x,1+a_3.y,1+a_3.z,1+a_3.w);
		pv->uv4.set	(1+a_4.x,1+a_4.y,1+a_4.z,1+a_4.w);
		pv->uv5.set	(1+a_5.x,1+a_5.y,1+a_5.z,1+a_5.w);
		pv->uv6.set	(1+a_6.x,1+a_6.y,1+a_6.z,1+a_6.w);
		pv->uv7.set	(1+a_7.x,1+a_7.y,1+a_7.z,1+a_7.w);
		pv++;								

		// 3 - RT
		pv->p.set	(float(_w+EPS),	EPS,			EPS,1.f);	
		pv->uv0.set	(1+a_0.x,a_0.y,0,0);	
		pv->uv1.set	(1+a_1.x,a_1.y,a_1.z,1+a_1.w);	
		pv->uv2.set	(1+a_2.x,a_2.y,a_2.z,1+a_2.w);	
		pv->uv3.set	(1+a_3.x,a_3.y,a_3.z,1+a_3.w);
		pv->uv4.set	(1+a_4.x,a_4.y,a_4.z,1+a_4.w);
		pv->uv5.set	(1+a_5.x,a_5.y,a_5.z,1+a_5.w);
		pv->uv6.set	(1+a_6.x,a_6.y,a_6.z,1+a_6.w);
		pv->uv7.set	(1+a_7.x,a_7.y,a_7.z,1+a_7.w);
		pv++;							 
		RCache.Vertex.Unlock		(4,g_bloom_filter->vb_stride);

		// Perform filtering
		Fvector4	w0,w1;
		CalcGauss_k7				(w0,w1,ps_r2_ls_bloom_kernel,1.f);
		RCache.set_RT				(rt_Bloom_1->pRT,		0);
		RCache.set_Element			(s_bloom->E[2]);
		RCache.set_ca				("weight", 0,			w0);
		RCache.set_ca				("weight", 1,			w1);
		RCache.set_Geometry			(g_bloom_filter);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// 
	{
		// Render skybox/skydome into Bloom2.rgbx
		RCache.set_RT						(rt_Bloom_2->pRT,		0);
		RImplementation.rmFar				();
		pCreator->Environment.RenderFirst	();
		RImplementation.rmNormal			();
	}
}

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
}

#include "stdafx.h"

void	CRenderTarget::phase_accumulator()
{
	// Targets
	dwWidth								= Device.dwWidth;
	dwHeight							= Device.dwHeight;
	RCache.set_RT						(rt_Accumulator->pRT,	0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(HW.pBaseZB);
	RImplementation.rmNormal			();

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

	// Clear and mask	- only once per frame
	// Assuming next usage will be for directional light - apply mask 
	if (dwAccumulatorClearMark==Device.dwFrame)	return;
	dwAccumulatorClearMark				= Device.dwFrame;
	dwLightMarkerID						= 5;	// start from 5, increment in 2 units
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L));

	// Stencil	- draw only where stencil >= 0x1
	// Stencil	- increment value if both (stencil,aref) pass
	// Stencil	- result -> 0x2 where pixel can be potentialy lighted/shadowed
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x03				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0x01				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0x02				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_REPLACE));	
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));

	// Assuming next usage will be for directional light
	u32		Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);

	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Fill vertex buffer
	float	d_Z	= EPS_S, d_W = 1.f;
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);
	RCache.set_Geometry			(g_combine);
	RCache.set_Shader			(s_accum_direct_mask);

	// Constants
	Fvector		L_dir;
	Device.mView.transform_dir	(L_dir,RImplementation.Lights.sun_dir);
	L_dir.invert				();
	L_dir.normalize				();
	RCache.set_c				("light_direction",	L_dir.x,L_dir.y,L_dir.z,0.f);

	// Calculate light-brightness
	Fvector		L_clr;	
	L_clr.div					(RImplementation.Lights.sun_color, ps_r2_ls_dynamic_range);
	float		L_max			= _max(_max(L_clr.x,L_clr.y),L_clr.z);
	float		L_mag			= L_clr.magnitude()/_sqrt(3.f);
	float		L_gray			= (L_clr.x + L_clr.y + L_clr.z)/3.f;
	float		L_brightness	= (L_max+L_mag+L_gray)/3.f;		// maximal brightness at dot(L,N)=1
	s32			A_clip;
	if (L_brightness<EPS_S)		A_clip	= 254;
	else						
	{
		float		L_clip			= ps_r2_ls_dclip / L_brightness;
		A_clip						= _max(4,iFloor(L_clip));
	}

	// Render
	CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_ALPHAREF,			A_clip	));
	CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	0		));
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ));

	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x02				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0x00				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));
}

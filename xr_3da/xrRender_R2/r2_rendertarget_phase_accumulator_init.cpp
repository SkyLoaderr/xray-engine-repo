#include "stdafx.h"

void CRenderTarget::phase_accumulator_init()
{
	dwLightMarkerID								= 5;		// start from 5, increment in 2 units
	u32		clr4clear							= color_rgba(0,0,0,0);	// 0x00
	CHK_DX	(HW.pDevice->Clear					( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));

	// Render emissive geometry
	{
		// Stencil - write 0x0 at pixel pos
		RCache.set_Stencil							( TRUE,D3DCMP_ALWAYS,0x00,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.set_CullMode							(CULL_CCW);
		RCache.set_ColorWriteEnable					();
		RImplementation.r_dsgraph_render_emissive	();
	}

	// Common
	u32		Offset						= 0;
	float	d_Z							= EPS_S;
	float	d_W							= 1.f;
	Fvector2							p0,p1;

	// ***** Prepare mask for skybox *****
	// Render white quad where stencil = 0
	if (0)
	{
		RCache.set_Stencil			(TRUE,D3DCMP_EQUAL,0x00,0xff,0x00);

		// Assuming next usage will be for directional light
		u32		C					= color_rgba	(255,255,255,255);
		float	_w					= float			(Device.dwWidth);
		float	_h					= float			(Device.dwHeight);

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, 0, 0);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, 0, 0);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, 0, 0);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, 0, 0);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);
		RCache.set_Element			(s_accum_mask->E[0]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	//  *****  Prepare mask for skybox  *****
	// ***** Downsample into bloom2.rgba *****
	if (0)
	{
		// 1. nv3x - ZB doesn't help
		// 2. nv3x - Viewport doesn't help
		// 3. nv3x - ZEnable doesn't help
		u_setrt								(rt_Bloom_2,NULL,NULL,NULL);				// No need for ZBuffer at all
		CHK_DX	(HW.pDevice->SetRenderState	( D3DRS_ZENABLE,	FALSE				));
		RCache.set_CullMode					(CULL_NONE);
		RCache.set_Stencil					(FALSE);

		// Assuming next usage will be for directional light
		u32		C					= color_rgba	(255,255,255,255);
		float	_w					= float			(Device.dwWidth/2);
		float	_h					= float			(Device.dwHeight/2);
		float	tw					= float			(Device.dwWidth);
		float	th					= float			(Device.dwHeight);
		p0.set						(1.f/tw, 1.f/th);
		p1.set						((tw+1.f)/tw, (th+1.f)/th );

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);
		RCache.set_Element			(s_accum_mask->E[1]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		CHK_DX	(HW.pDevice->SetRenderState	( D3DRS_ZENABLE,		TRUE				));
	}

	// ***** Assuming next usage will be for directional light *****
	// Stencil	- draw only where stencil >= 0x1
	// Stencil	- increment value if both (stencil,aref) pass
	// Stencil	- result -> 0x2 where pixel can be potentialy lighted/shadowed
	if (0 && ps_r2_ls_flags.test(R2FLAG_SUN))
	{
		// Restore targets
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x03,0x01,0x02,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);

		// Assuming next usage will be for directional light
		u32		C					= color_rgba	(255,255,255,255);
		float	_w					= float			(Device.dwWidth);
		float	_h					= float			(Device.dwHeight);

		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);
		RCache.set_Element			(s_accum_mask->E[2]);

		// Calculate light-brightness
		Fcolor		L_sunc			= RImplementation.Lights.sun->color; 
		Fvector		L_clr;			L_clr.set(L_sunc.r,L_sunc.g,L_sunc.b).div(ps_r2_ls_dynamic_range);
		float		L_max			= _max(_max(L_clr.x,L_clr.y),L_clr.z);
		float		L_mag			= L_clr.magnitude()/_sqrt(3.f);
		float		L_gray			= (L_clr.x + L_clr.y + L_clr.z)/3.f;
		float		L_brightness	= (L_max+L_mag+L_gray)/3.f;		// maximal brightness at dot(L,N)=1

		// Constants
		Fvector		L_dir;
		Device.mView.transform_dir	(L_dir,RImplementation.Lights.sun->direction);
		L_dir.invert				();
		L_dir.set_length			(L_brightness);
		// RCache.set_c				("light_direction",	L_dir.x,L_dir.y,L_dir.z,0.f);

		// Render
		RCache.set_ColorWriteEnable	(FALSE);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		RCache.set_ColorWriteEnable	();
	}
}

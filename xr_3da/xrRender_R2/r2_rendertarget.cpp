#include "stdafx.h"

void	CRenderTarget::OnDeviceCreate	()
{
	u32	w=Device.dwWidth, h=Device.dwHeight;

	rt_Position		= Device.Shader._CreateRT	(r2_RT_P,		w,h,D3DFMT_A16B16G16R16F);
	rt_Normal		= Device.Shader._CreateRT	(r2_RT_N,		w,h,D3DFMT_A16B16G16R16F);
	rt_Color		= Device.Shader._CreateRT	(r2_RT_D_G,		w,h,D3DFMT_A16B16G16R16);
	rt_Accumulator	= Device.Shader._CreateRT	(r2_RT_accum,	w,h,D3DFMT_A8R8G8B8);
	rt_Bloom_1		= Device.Shader._CreateRT	(r2_RT_bloom1,	w,h,D3DFMT_A8R8G8B8);
	rt_Bloom_2		= Device.Shader._CreateRT	(r2_RT_bloom2,	w,h,D3DFMT_A8R8G8B8);

	s_combine_dbg_Position		= Device.Shader.Create		("effects\\screen_set",		r2_RT_P);
	s_combine_dbg_Normal		= Device.Shader.Create		("effects\\screen_set",		r2_RT_N);
	s_combine_dbg_Color			= Device.Shader.Create		("effects\\screen_set",		r2_RT_D_G);
	s_combine_dbg_Accumulator	= Device.Shader.Create		("effects\\screen_set",		r2_RT_accum);

	g_combine					= Device.Shader.CreateGeom	(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
}

void	CRenderTarget::OnDeviceDestroy	()
{
	Device.Shader.DeleteGeom	(g_combine);

	Device.Shader.Delete		(s_combine_dbg_Position);
	Device.Shader.Delete		(s_combine_dbg_Normal);
	Device.Shader.Delete		(s_combine_dbg_Color);
	Device.Shader.Delete		(s_combine_dbg_Accumulator);

	Device.Shader._DeleteRT		(rt_Bloom_2		);
	Device.Shader._DeleteRT		(rt_Bloom_1		);
	Device.Shader._DeleteRT		(rt_Accumulator	);
	Device.Shader._DeleteRT		(rt_Color		);
	Device.Shader._DeleteRT		(rt_Normal		);
	Device.Shader._DeleteRT		(rt_Position	);
}

void	CRenderTarget::phase_scene		()
{
	RCache.set_RT				(rt_Position->pRT,		0);
	RCache.set_RT				(rt_Normal->pRT,		1);
	RCache.set_RT				(rt_Color->pRT,			2);
	RCache.set_ZB				(HW.pBaseZB);
	Device.Clear				();
}

void	CRenderTarget::phase_combine	()
{
	RCache.set_RT				(HW.pBaseRT,			0);
	RCache.set_RT				(0,						1);
	RCache.set_RT				(0,						2);
	RCache.set_ZB				(HW.pBaseZB);

	// Draw full-screen quad textured with our scene image
	u32	Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);

	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	
	// Fill vertex buffer
	FVF::TL* pv			= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock(4,g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Shader			(s_combine_dbg_Normal);
	RCache.set_Geometry			(g_combine);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

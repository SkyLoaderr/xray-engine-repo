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

	s_combine_dbg_Color			= Device.Shader.Create		("effects\\screen_set",		r2_RT_D_G);
	s_combine_dbg_Normal		= Device.Shader.Create		("effects\\screen_set",		r2_RT_D_G);
	s_combine_dbg_Accumulator	= Device.Shader.Create		("effects\\screen_set",		r2_RT_D_G);
}

void	CRenderTarget::OnDeviceDestroy	()
{
	Device.Shader.Delete		(s_combine_dbg_Color);
	Device.Shader.Delete		(s_combine_dbg_Normal);
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
}

void	CRenderTarget::phase_combine	()
{
	RCache.set_RT				(HW.pBaseRT,			0);
	RCache.set_RT				(0,						1);
	RCache.set_RT				(0,						2);
	RCache.set_ZB				(HW.pBaseZB);
}

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
}

void	CRenderTarget::OnDeviceDestroy	()
{
	Device.Shader._DeleteRT		(rt_Bloom_2		);
	Device.Shader._DeleteRT		(rt_Bloom_1		);
	Device.Shader._DeleteRT		(rt_Accumulator	);
	Device.Shader._DeleteRT		(rt_Color		);
	Device.Shader._DeleteRT		(rt_Normal		);
	Device.Shader._DeleteRT		(rt_Position	);
}

#include "stdafx.h"
#include "blender_light_direct.h"

void	CRenderTarget::OnDeviceCreate	()
{
	dwAccumulatorClearMark			= 0;

	// blenders
	b_accum_direct					= xr_new<CBlender_accum_direct>		();

	//	NORMAL
	{
		u32	w=Device.dwWidth, h=Device.dwHeight;
		rt_Position					= Device.Shader._CreateRT	(r2_RT_P,		w,h,D3DFMT_A16B16G16R16F);
		rt_Normal					= Device.Shader._CreateRT	(r2_RT_N,		w,h,D3DFMT_A16B16G16R16F);
		rt_Color					= Device.Shader._CreateRT	(r2_RT_D_G,		w,h,D3DFMT_A16B16G16R16);
		rt_Accumulator				= Device.Shader._CreateRT	(r2_RT_accum,	w,h,D3DFMT_A8R8G8B8);
		rt_Bloom_1					= Device.Shader._CreateRT	(r2_RT_bloom1,	w,h,D3DFMT_A8R8G8B8);
		rt_Bloom_2					= Device.Shader._CreateRT	(r2_RT_bloom2,	w,h,D3DFMT_A8R8G8B8);
	}

	// DIRECT
	{
		u32	w=DSM_size, h=DSM_size;

		R_CHK						(HW.pDevice->CreateDepthStencilSurface	(w,h,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&rt_smap_d_ZB,NULL));
		rt_smap_d					= Device.Shader._CreateRT	(r2_RT_smap_d,			w,h,D3DFMT_R32F);
		s_smap_d_debug				= Device.Shader.Create		("effects\\screen_set",	r2_RT_smap_d);
		s_accum_direct				= Device.Shader.Create_B	(b_accum_direct,		"r2\\accum_direct");
		g_smap_d_debug				= Device.Shader.CreateGeom	(FVF::F_TL,				RCache.Vertex.Buffer(), RCache.QuadIB);
	}

	// COMBINE
	{
		s_combine_dbg_Position		= Device.Shader.Create		("effects\\screen_set",		r2_RT_P);
		s_combine_dbg_Normal		= Device.Shader.Create		("effects\\screen_set",		r2_RT_N);
		s_combine_dbg_Color			= Device.Shader.Create		("effects\\screen_set",		r2_RT_D_G);
		s_combine_dbg_Accumulator	= Device.Shader.Create		("effects\\screen_set",		r2_RT_accum);
		g_combine					= Device.Shader.CreateGeom	(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
	}

	// 
	dwWidth		= Device.dwWidth;
	dwHeight	= Device.dwHeight;
}

void	CRenderTarget::OnDeviceDestroy	()
{
	// COMBINE
	Device.Shader.DeleteGeom	(g_combine);
	Device.Shader.Delete		(s_combine_dbg_Position);
	Device.Shader.Delete		(s_combine_dbg_Normal);
	Device.Shader.Delete		(s_combine_dbg_Color);
	Device.Shader.Delete		(s_combine_dbg_Accumulator);

	// DIRECT
	Device.Shader.DeleteGeom	(g_smap_d_debug	);
	Device.Shader.Delete		(s_accum_direct	);
	Device.Shader.Delete		(s_smap_d_debug	);
	Device.Shader._DeleteRT		(rt_smap_d		);
	_RELEASE					(rt_smap_d_ZB	);

	// NORMAL
	Device.Shader._DeleteRT		(rt_Bloom_2		);
	Device.Shader._DeleteRT		(rt_Bloom_1		);
	Device.Shader._DeleteRT		(rt_Accumulator	);
	Device.Shader._DeleteRT		(rt_Color		);
	Device.Shader._DeleteRT		(rt_Normal		);
	Device.Shader._DeleteRT		(rt_Position	);

	// Blenders
	xr_delete					(b_accum_direct	);
}


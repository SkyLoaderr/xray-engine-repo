#include "stdafx.h"
#include "light_render_direct.h"

void	CRenderTarget::OnDeviceCreate	()
{
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
}

void	CRenderTarget::phase_scene		()
{
	dwWidth						= Device.dwWidth;
	dwHeight					= Device.dwHeight;
	RCache.set_RT				(rt_Position->pRT,		0);
	RCache.set_RT				(rt_Normal->pRT,		1);
	RCache.set_RT				(rt_Color->pRT,			2);
	RCache.set_ZB				(HW.pBaseZB);
	RImplementation.rmNormal	();
	Device.Clear				();
}
void	CRenderTarget::phase_smap_direct()
{
	dwWidth						= DSM_size;
	dwHeight					= DSM_size;
	RCache.set_RT				(rt_smap_d->pRT,		0);
	RCache.set_RT				(NULL,					1);
	RCache.set_RT				(NULL,					2);
	RCache.set_ZB				(rt_smap_d_ZB);
	RImplementation.rmNormal	();
}

void	CRenderTarget::phase_accumulator()
{
	dwWidth						= Device.dwWidth;
	dwHeight					= Device.dwHeight;
	RCache.set_RT				(rt_Accumulator->pRT,	0);
	RCache.set_RT				(NULL,					1);
	RCache.set_RT				(NULL,					2);
	RCache.set_ZB				(HW.pBaseZB);
	RImplementation.rmNormal	();
}

void	CRenderTarget::phase_combine	()
{
	// 
	dwWidth						= Device.dwWidth;
	dwHeight					= Device.dwHeight;
	RCache.set_RT				(HW.pBaseRT,			0);
	RCache.set_RT				(NULL,					1);
	RCache.set_RT				(NULL,					2);
	RCache.set_ZB				(HW.pBaseZB);
	RImplementation.rmNormal	();

	// Draw full-screen quad textured with our scene image
	u32	Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
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

	// Draw COLOR
	RCache.set_Shader			(s_combine_dbg_Color);
	RCache.set_Geometry			(g_combine);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

#include "StdAfx.h"
#include "light_render_direct.h"

CLight_Render_Direct::CLight_Render_Direct	()
{
}

CLight_Render_Direct::~CLight_Render_Direct	()
{
}

void	CLight_Render_Direct::Create		()
{
	u32	w=DSM_size, h=DSM_size;

	R_CHK		(HW.pDevice->CreateDepthStencilSurface	(w,h,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&rt_ZB,NULL));
	rt_smap		= Device.Shader._CreateRT	(r2_RT_smap_d,	w,h,D3DFMT_R32F);
	s_debug		= Device.Shader.Create		("effects\\screen_set",		r2_RT_smap_d);
	g_debug		= Device.Shader.CreateGeom	(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
}

void	CLight_Render_Direct::Destroy		()
{
	Device.Shader.DeleteGeom	(g_debug);
	Device.Shader.Delete		(s_debug);
	Device.Shader._DeleteRT		(rt_smap);
	_RELEASE					(rt_ZB);
}

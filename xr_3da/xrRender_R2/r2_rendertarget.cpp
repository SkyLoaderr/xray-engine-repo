#include "stdafx.h"
#include "blender_light_mask.h"
#include "blender_light_direct.h"
#include "blender_light_point.h"
#include "blender_light_point_uns.h"
#include "blender_light_spot.h"
#include "blender_combine.h"
#include "blender_bloom_build.h"


void	CRenderTarget::u_setrt			(CRT* _1, CRT* _2, CRT* _3, IDirect3DSurface9* zb)
{
	if (RImplementation.b_nv3x)
	{
		VERIFY								(_1);
		VERIFY								(0==_2);
		VERIFY								(0==_3);

		// Use only one RT + ZB
		RCache.set_RT						(_1->pRT,	0);
		RCache.set_ZB						(zb);
	} else {
		VERIFY								(_1);

		if (_1) RCache.set_RT(_1->pRT,	0); else RCache.set_RT(NULL,0);
		if (_2) RCache.set_RT(_2->pRT,	1); else RCache.set_RT(NULL,1);
		if (_3) RCache.set_RT(_3->pRT,	2); else RCache.set_RT(NULL,2);
		RCache.set_ZB						(zb);
	}
}

void	CRenderTarget::u_setrt			(IDirect3DSurface9* _1, IDirect3DSurface9* _2, IDirect3DSurface9* _3, IDirect3DSurface9* zb)
{
	if (RImplementation.b_nv3x)
	{
		VERIFY								(_1);
		VERIFY								(0==_2);
		VERIFY								(0==_3);

		// Use only one RT + ZB
		RCache.set_RT						(_1,	0);
		RCache.set_ZB						(zb);
	} else {
		VERIFY								(_1);

		RCache.set_RT						(_1,	0);
		RCache.set_RT						(_2,	1);
		RCache.set_RT						(_3,	2);
		RCache.set_ZB						(zb);
	}
}

void	CRenderTarget::OnDeviceCreate	()
{
	dwAccumulatorClearMark			= 0;
	Device.Shader.Evict				();

	// Blenders
	b_accum_mask					= xr_new<CBlender_accum_direct_mask>	();
	b_accum_direct					= xr_new<CBlender_accum_direct>			();
	b_accum_point_s					= xr_new<CBlender_accum_point>			();
	b_accum_point_uns				= xr_new<CBlender_accum_point_uns>		();
	b_accum_spot_s					= xr_new<CBlender_accum_spot>			();
	b_bloom							= xr_new<CBlender_bloom_build>			();
	b_combine						= xr_new<CBlender_combine>				();

	//	NORMAL
	if (RImplementation.b_nv3x)
	{
		u32	w=Device.dwWidth, h=Device.dwHeight;
		rt_Deffer					= Device.Shader._CreateRT	(r2_RT_DEFFER,	w,h,(D3DFORMAT)MAKEFOURCC('N','V','E','3'));
		R_ASSERT					(rt_Deffer);
	} 
	else
	{
		u32	w=Device.dwWidth, h=Device.dwHeight;
		rt_Position					= Device.Shader._CreateRT	(r2_RT_P,		w,h,D3DFMT_A16B16G16R16F);
		rt_Normal					= Device.Shader._CreateRT	(r2_RT_N_H,		w,h,D3DFMT_A16B16G16R16F);
		rt_Color					= Device.Shader._CreateRT	(r2_RT_D_G,		w,h,D3DFMT_A16B16G16R16);
	}

	// NORMAL-part2
	{
		u32	w=Device.dwWidth, h=Device.dwHeight;
		rt_Accumulator				= Device.Shader._CreateRT	(r2_RT_accum,	w,h,D3DFMT_A8R8G8B8);
		rt_Generic					= Device.Shader._CreateRT	(r2_RT_generic,	w,h,D3DFMT_A8R8G8B8);
	}

	// DIRECT
	{
		u32	w=DSM_size, h=DSM_size;

		R_CHK						(HW.pDevice->CreateDepthStencilSurface	(w,h,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&rt_smap_d_ZB,NULL));
		rt_smap_d					= Device.Shader._CreateRT	(r2_RT_smap_d,				w,h,D3DFMT_R32F);
		s_accum_mask				= Device.Shader.Create_B	(b_accum_mask,				"r2\\accum_mask");
		s_accum_direct				= Device.Shader.Create_B	(b_accum_direct,			"r2\\accum_direct");
	}

	// POINT
	{
		u32 size = PSM_size;
		R_CHK						(HW.pDevice->CreateDepthStencilSurface	(size,size,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&rt_smap_p_ZB,NULL));
		rt_smap_p					= Device.Shader._CreateRTC	(r2_RT_smap_p,				size,D3DFMT_R32F);
		s_accum_point_s				= Device.Shader.Create_B	(b_accum_point_s,			"r2\\accum_point_s");
		s_accum_point_uns			= Device.Shader.Create_B	(b_accum_point_uns,			"r2\\accum_point_uns");

		accum_point_geom_create		();
		g_accum_point				= Device.Shader.CreateGeom	(D3DFVF_XYZ,				g_accum_point_vb, g_accum_point_ib);
	}

	// SPOT
	{
		u32 size = SSM_size;
		R_CHK						(HW.pDevice->CreateDepthStencilSurface	(size,size,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&rt_smap_s_ZB,NULL));
		rt_smap_s					= Device.Shader._CreateRT	(r2_RT_smap_s,				size,size,D3DFMT_R32F);
		s_accum_spot_s				= Device.Shader.Create_B	(b_accum_spot_s,			"r2\\accum_spot_s");
	}

	// BLOOM
	{
		u32	w=Device.dwWidth/2, h=Device.dwHeight/2;
		u32 fvf_build				= D3DFVF_XYZRHW|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)|D3DFVF_TEXCOORDSIZE2(2)|D3DFVF_TEXCOORDSIZE2(3);
		u32 fvf_filter				= D3DFVF_XYZRHW|D3DFVF_TEX8|D3DFVF_TEXCOORDSIZE4(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE4(2)|D3DFVF_TEXCOORDSIZE4(3)|D3DFVF_TEXCOORDSIZE4(4)|D3DFVF_TEXCOORDSIZE4(5)|D3DFVF_TEXCOORDSIZE4(6)|D3DFVF_TEXCOORDSIZE4(7);
		rt_Bloom_1					= Device.Shader._CreateRT	(r2_RT_bloom1,	w,h,D3DFMT_A8R8G8B8);
		rt_Bloom_2					= Device.Shader._CreateRT	(r2_RT_bloom2,	w,h,D3DFMT_A8R8G8B8);
		g_bloom_build				= Device.Shader.CreateGeom	(fvf_build,		RCache.Vertex.Buffer(), RCache.QuadIB);
		g_bloom_filter				= Device.Shader.CreateGeom	(fvf_filter,	RCache.Vertex.Buffer(), RCache.QuadIB);
		s_bloom_dbg_1				= Device.Shader.Create		("effects\\screen_set",		r2_RT_bloom1);
		s_bloom_dbg_2				= Device.Shader.Create		("effects\\screen_set",		r2_RT_bloom2);
		s_bloom						= Device.Shader.Create_B	(b_bloom,					"r2\\bloom");
	}

	// COMBINE
	{
		s_combine					= Device.Shader.Create_B	(b_combine,					"r2\\combine");
		s_combine_dbg_Normal		= Device.Shader.Create		("effects\\screen_set",		r2_RT_N_H);
		s_combine_dbg_Accumulator	= Device.Shader.Create		("effects\\screen_set",		r2_RT_accum);
		g_combine					= Device.Shader.CreateGeom	(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
	}

	// Build textures
	{
		// Build material(s)
		{
			// Surface
			R_CHK						(D3DXCreateTexture(HW.pDevice,TEX_material_LdotN,TEX_material_LdotH,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&t_material_surf));
			t_material					= Device.Shader._CreateTexture(r2_material);
			t_material->surface_set		(t_material_surf);

			// Fill it (addr: x=dot(L,N),y=dot(L,H))
			D3DLOCKED_RECT				R;
			R_CHK						(t_material_surf->LockRect	(0,&R,0,0));
			for (u32 y=0; y<TEX_material_LdotH; y++)
			{
				for (u32 x=0; x<TEX_material_LdotN; x++)
				{
					u32*	p	=	(u32*)		(LPBYTE (R.pBits) + y*R.Pitch + x*4);
					float	ld	=	float(x)	/ float	(TEX_material_LdotN-1);
					float	ls	=	float(y)	/ float	(TEX_material_LdotH-1);
							ls	*=	powf		(ld,1/4);				// minimize specular where diffuse near zero
					s32		_d	=	iFloor		(ld*255.5f);			clamp(_d,0,255);
					s32		_s	=	iFloor		(pow(ls,32.f)*255.5f);	clamp(_s,0,255);
					*p			=	color_rgba	(_d,_d,_d,_s);
				}
			}
			R_CHK						(t_material_surf->UnlockRect	(0));
		}

		// Build shadow2fade
		{
			// Surface
			R_CHK						(D3DXCreateTexture(HW.pDevice,TEX_ds2_fade_size,1,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&t_ds2fade_surf));
			t_ds2fade					= Device.Shader._CreateTexture(r2_ds2_fade);
			t_ds2fade->surface_set		(t_ds2fade_surf);

			// Fill it (addr:depth/DSM_distance_2; res:x=mul,w=add)
			D3DLOCKED_RECT				R;
			R_CHK						(t_ds2fade_surf->LockRect	(0,&R,0,0));
			for (u32 x=0; x<TEX_ds2_fade_size; x++)
			{
				u32*	p		= (u32*)	(LPBYTE (R.pBits) + x*4);
				float	frac	= float(x)  / float	(TEX_ds2_fade_size-1);
				float	len		= frac		* DSM_distance_2;
				float	alpha_l	= len		/ DSM_distance_1;	clamp	(alpha_l,	0.f,1.f);
				float	fade_l	= len		/ DSM_distance_2;	clamp	(fade_l,	0.f,1.f);
				float	alpha	= _sqr(alpha_l);
				float	fade	= _sqr(fade_l);
				float	_mul_	= (1-fade)*alpha;
				float	_add_	= fade*alpha;
				s32		_mul_i	= iFloor	(_mul_*255.5f);		clamp	(_mul_i,0,255);
				s32		_add_i 	= iFloor	(_add_*255.5f);		clamp	(_add_i,0,255);
				*p				= color_rgba(_mul_i,_mul_i,_mul_i,_add_i);
			}
			R_CHK						(t_ds2fade_surf->UnlockRect	(0));
		}
	}

	// 
	dwWidth		= Device.dwWidth;
	dwHeight	= Device.dwHeight;
}

void	CRenderTarget::OnDeviceDestroy	()
{
	// Textures
	t_ds2fade->surface_set		(NULL);
	_RELEASE					(t_ds2fade_surf);
	t_material->surface_set		(NULL);
	_RELEASE					(t_material_surf);

	// COMBINE
	Device.Shader.DeleteGeom	(g_combine				);
	Device.Shader.Delete		(s_combine_dbg_Normal	);
	Device.Shader.Delete		(s_combine_dbg_Accumulator);
	Device.Shader.Delete		(s_combine				);

	// BLOOM
	Device.Shader._DeleteRT		(rt_Bloom_2				);
	Device.Shader._DeleteRT		(rt_Bloom_1				);
	Device.Shader.Delete		(s_bloom				);
	Device.Shader.Delete		(s_bloom_dbg_2			);
	Device.Shader.Delete		(s_bloom_dbg_1			);
	Device.Shader.DeleteGeom	(g_bloom_filter			);
	Device.Shader.DeleteGeom	(g_bloom_build			);

	// SPOT
	Device.Shader.Delete		(s_accum_spot_s			);
	Device.Shader._DeleteRT		(rt_smap_s				);
	_RELEASE					(rt_smap_s_ZB);

	// POINT
	Device.Shader.DeleteGeom	(g_accum_point			);
	accum_point_geom_destroy	();
	Device.Shader.Delete		(s_accum_point_s		);
	Device.Shader.Delete		(s_accum_point_uns		);
	Device.Shader._DeleteRTC	(rt_smap_p				);
	_RELEASE					(rt_smap_p_ZB			);

	// DIRECT
	Device.Shader.Delete		(s_accum_direct			);
	Device.Shader.Delete		(s_accum_mask			);
	Device.Shader._DeleteRT		(rt_smap_d				);
	_RELEASE					(rt_smap_d_ZB			);

	// NORMAL
	Device.Shader._DeleteRT		(rt_Generic				);
	Device.Shader._DeleteRT		(rt_Accumulator			);

	Device.Shader._DeleteRT		(rt_Color				);
	Device.Shader._DeleteRT		(rt_Normal				);
	Device.Shader._DeleteRT		(rt_Position			);
	Device.Shader._DeleteRT		(rt_Deffer				);

	// Blenders
	xr_delete					(b_combine				);
	xr_delete					(b_bloom				);
	xr_delete					(b_accum_spot_s			);
	xr_delete					(b_accum_spot_uns		);
	xr_delete					(b_accum_point_uns		);
	xr_delete					(b_accum_point_s		);
	xr_delete					(b_accum_direct			);
	xr_delete					(b_accum_mask			);
}

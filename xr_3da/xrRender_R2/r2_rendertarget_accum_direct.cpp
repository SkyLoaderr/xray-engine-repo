#include "stdafx.h"

void CRenderTarget::accum_direct		(u32 sub_phase)
{
	// Choose normal code-path or filtered
	phase_accumulator					();
	if (RImplementation.o.sunfilter)	{
		accum_direct_f	(sub_phase);
		return			;
	}

	// *** assume accumulator setted up ***
	light*			fuckingsun			= RImplementation.Lights.sun_adapted	;

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	float	_w					= float			(Device.dwWidth);
	float	_h					= float			(Device.dwHeight);
	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	float	d_Z	= EPS_S, d_W = 1.f;

	// Common constants (light-related)
	Fvector		L_dir,L_clr;	float L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	if (SE_SUN_NEAR==sub_phase)	//.
	{
		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		float	intensity			= 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
		Fvector	dir					= L_dir;
				dir.normalize().mul	(- _sqrt(intensity+EPS));
		RCache.set_Element			(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

		// if (stencil>=1 && aref_pass)	stencil = light_id
		RCache.set_ColorWriteEnable	(FALSE		);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// nv-stencil recompression
	if (RImplementation.o.nvstencil  && (SE_SUN_NEAR==sub_phase))	u_stencil_optimize();	//. driver bug?

	// Perform lighting
	{
		phase_accumulator					();
		RCache.set_CullMode					(CULL_NONE	);
		RCache.set_ColorWriteEnable			();

		// texture adjustment matrix
		float			fTexelOffs			= (.5f / float(RImplementation.o.smapsize));
		float			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_scale:ps_r2_sun_depth_far_scale;
		float			fBias				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_bias:ps_r2_sun_depth_far_bias;
		Fmatrix			m_TexelAdjust		= 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		Fmatrix				m_shadow;
		{
			FPU::m64r		();
			Fmatrix			xf_invview;		xf_invview.invert	(Device.mView)	;
			Fmatrix			xf_project;		xf_project.mul		(m_TexelAdjust,fuckingsun->X.D.combine);
			m_shadow.mul	(xf_project,	xf_invview);

			// tsm-bias
			if (SE_SUN_FAR == sub_phase)
			{
				Fvector		bias;	bias.mul		(L_dir,ps_r2_sun_tsm_bias);
				Fmatrix		bias_t;	bias_t.translate(bias);
				m_shadow.mulB		(bias_t);
			}
			FPU::m24r		();
		}

		// Make jitter texture
		Fvector2					j0,j1;
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);
		float	offset				= (.5f / float(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y, j0.x, j1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y, j0.x, j0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y, j1.x, j1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y, j1.x, j0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);

		// setup
		RCache.set_Element			(s_accum_direct->E[sub_phase]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}

void CRenderTarget::accum_direct_blend	()
{
	// blend-copy
	if (!RImplementation.o.fp16_blend)	{
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_accum_mask->E[SE_MASK_ACCUM_2D]	);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2	);
	}
	Target->dwLightMarkerID				+= 2;
}

void CRenderTarget::accum_direct_f		(u32 sub_phase)
{
	// Select target
	if (SE_SUN_LUMINANCE==sub_phase)	{
		accum_direct_lum	();
		return				;
	}
	phase_accumulator					();
	u_setrt								(rt_Generic_0,NULL,NULL,HW.pBaseZB);

	// *** assume accumulator setted up ***
	light*			fuckingsun			= RImplementation.Lights.sun_adapted	;

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	float	_w					= float			(Device.dwWidth);
	float	_h					= float			(Device.dwHeight);
	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	float	d_Z	= EPS_S, d_W = 1.f;

	// Common constants (light-related)
	Fvector		L_dir,L_clr;	float L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	if (SE_SUN_NEAR==sub_phase)	//.
	{
		// For sun-filter - clear to zero
		CHK_DX	(HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L));

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		float	intensity			= 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
		Fvector	dir					= L_dir;
		dir.normalize().mul	(- _sqrt(intensity+EPS));
		RCache.set_Element			(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

		// if (stencil>=1 && aref_pass)	stencil = light_id
		RCache.set_ColorWriteEnable	(FALSE		);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// nv-stencil recompression
	if (RImplementation.o.nvstencil  && (SE_SUN_NEAR==sub_phase))	u_stencil_optimize();	//. driver bug?

	// Perform lighting
	{
		u_setrt								(rt_Generic_0,NULL,NULL,HW.pBaseZB);  // enshure RT setup
		RCache.set_CullMode					(CULL_NONE	);
		RCache.set_ColorWriteEnable			();

		// texture adjustment matrix
		float			fTexelOffs			= (.5f / float(RImplementation.o.smapsize));
		float			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_scale:ps_r2_sun_depth_far_scale;
		float			fBias				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_bias:ps_r2_sun_depth_far_bias;
		Fmatrix			m_TexelAdjust		= 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		Fmatrix				m_shadow;
		{
			FPU::m64r		();
			Fmatrix			xf_invview;		xf_invview.invert	(Device.mView)	;
			Fmatrix			xf_project;		xf_project.mul		(m_TexelAdjust,fuckingsun->X.D.combine);
			m_shadow.mul	(xf_project,	xf_invview);

			// tsm-bias
			if (SE_SUN_FAR == sub_phase)
			{
				Fvector		bias;	bias.mul		(L_dir,ps_r2_sun_tsm_bias);
				Fmatrix		bias_t;	bias_t.translate(bias);
				m_shadow.mulB		(bias_t);
			}
			FPU::m24r		();
		}

		// Make jitter texture
		Fvector2					j0,j1;
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);
		float	offset				= (.5f / float(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y, j0.x, j1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y, j0.x, j0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y, j1.x, j1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y, j1.x, j0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);

		// setup
		RCache.set_Element			(s_accum_direct->E[sub_phase]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}

void CRenderTarget::accum_direct_lum	()
{
	// Select target
	phase_accumulator					();

	// *** assume accumulator setted up ***
	light*			fuckingsun			= RImplementation.Lights.sun_adapted	;

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	float	_w					= float			(Device.dwWidth);
	float	_h					= float			(Device.dwHeight);
	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	float	d_Z	= EPS_S, d_W = 1.f;

	// Common constants (light-related)
	Fvector		L_dir,L_clr;	float L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// nv-stencil recompression
	/*
	if (RImplementation.o.nvstencil  && (SE_SUN_NEAR==sub_phase))	u_stencil_optimize();	//. driver bug?
	*/

	// Perform lighting
		RCache.set_CullMode					(CULL_NONE	);
		RCache.set_ColorWriteEnable			();

		// Make jitter texture
		Fvector2					j0,j1;
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);
		float	offset				= (.5f / float(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		struct v_aa	{
			Fvector4	p;
			Fvector2	uv0;
			Fvector2	uvJ;
			Fvector2	uv1;
			Fvector2	uv2;
			Fvector2	uv3;
			Fvector4	uv4;
			Fvector4	uv5;
		};
		float	smooth				= 0.6f;
		float	ddw					= smooth/_w;
		float	ddh					= smooth/_h;

		// Fill vertex buffer
		VERIFY	(sizeof(v_aa)==g_aa_AA->vb_stride);
		v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa_AA->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uvJ.set(j0.x, j1.y);pv->uv1.set(p0.x-ddw,p1.y-ddh);pv->uv2.set(p0.x+ddw,p1.y+ddh);pv->uv3.set(p0.x+ddw,p1.y-ddh);pv->uv4.set(p0.x-ddw,p1.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uvJ.set(j0.x, j0.y);pv->uv1.set(p0.x-ddw,p0.y-ddh);pv->uv2.set(p0.x+ddw,p0.y+ddh);pv->uv3.set(p0.x+ddw,p0.y-ddh);pv->uv4.set(p0.x-ddw,p0.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uvJ.set(j1.x, j1.y);pv->uv1.set(p1.x-ddw,p1.y-ddh);pv->uv2.set(p1.x+ddw,p1.y+ddh);pv->uv3.set(p1.x+ddw,p1.y-ddh);pv->uv4.set(p1.x-ddw,p1.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uvJ.set(j1.x, j0.y);pv->uv1.set(p1.x-ddw,p0.y-ddh);pv->uv2.set(p1.x+ddw,p0.y+ddh);pv->uv3.set(p1.x+ddw,p0.y-ddh);pv->uv4.set(p1.x-ddw,p0.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		RCache.Vertex.Unlock		(4,g_aa_AA->vb_stride);
		RCache.set_Geometry			(g_aa_AA);

		// setup
		RCache.set_Element			(s_accum_direct->E[SE_SUN_LUMINANCE]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

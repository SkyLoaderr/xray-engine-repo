#include "stdafx.h"

void CRenderTarget::accum_point		(light* L)
{
	RImplementation.stats.l_visible	++;

	ref_shader		shader			= L->s_point;
	if (!shader)	shader			= s_accum_point;

	// Common
	Fvector		L_pos;
	float		L_spec;
	float		L_R					= L->range;
	Fvector		L_clr;				L_clr.set(L->color.r,L->color.g,L->color.b);
	L_spec							= u_diffuse2s	(L_clr)/ps_r2_ls_dynamic_range;
	L_clr.div						(ps_r2_ls_dynamic_range);
	Device.mView.transform_tiny		(L_pos,L->position);

	// Xforms
	L->xform_calc					();
	RCache.set_xform_world			(L->m_xform);
	RCache.set_xform_view			(Device.mView);
	RCache.set_xform_project		(Device.mProject);
	enable_scissor					(L);

	// *****************************	Mask by stencil		*************************************
	// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
	// *** thus can cope without stencil clear with 127 lights
	// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
	RCache.set_Element				(s_accum_mask->E[SE_MASK_POINT]);			// masker
	RCache.set_ColorWriteEnable		(FALSE);

	// backfaces: if (stencil>=1 && zfail)	stencil = light_id
	RCache.set_CullMode				(CULL_CW);
	RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
	draw_volume						(L);

	// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
	RCache.set_CullMode				(CULL_CCW);
	RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
	draw_volume						(L);

	// nv-stencil recompression
	if (RImplementation.o.nvstencil)	u_stencil_optimize();

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	RCache.set_ColorWriteEnable				();
	RCache.set_CullMode						(CULL_CW);		// back
	/*
	if (bIntersect)	RCache.set_CullMode		(CULL_CW);		// back
	else			RCache.set_CullMode		(CULL_CCW);		// front
	*/

	// 2D texgen (texture adjustment matrix)
	float	_w						= float(Device.dwWidth);
	float	_h						= float(Device.dwHeight);
	float	o_w						= (.5f / _w);
	float	o_h						= (.5f / _h);
	Fmatrix			m_TexelAdjust	= {
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				1.0f,			0.0f,
		0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
	};
	Fmatrix			m_Tex;
	m_Tex.mul		(m_TexelAdjust,RCache.xforms.m_wvp);

	// Draw volume with projective texgen
	{
		// Select shader
		u32		_id					= 0;
		if (L->flags.bShadow)		{
			bool	bFullSize			= (L->X.S.size == SMAP_size);
			if (L->X.S.transluent)	_id	= SE_L_TRANSLUENT;
			else if		(bFullSize)	_id	= SE_L_FULLSIZE;
			else					_id	= SE_L_NORMAL;
		} else {
			_id						= SE_L_UNSHADOWED;
			//m_Shadow				= m_Lmap;
		}
		RCache.set_Element				(shader->E[ _id ]	);

		// Constants
		RCache.set_c					("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,1/(L_R*L_R));
		RCache.set_c					("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
		RCache.set_c					("m_texgen",		m_Tex);

		// Render if (stencil >= light_id && z-pass)
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
		draw_volume						(L);
	}

	dwLightMarkerID					+=	2;	// keep lowest bit always setted up
	CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));
}

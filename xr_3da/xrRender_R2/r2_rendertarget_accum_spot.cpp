#include "stdafx.h"

extern Fvector du_cone_vertices[DU_CONE_NUMVERTEX];

void CRenderTarget::accum_spot	(light* L)
{
	// *** assume accumulator setted up ***
	// *****************************	Mask by stencil		*************************************
	ref_shader		shader		= L->s_spot;
	if (!shader)	shader		= s_accum_spot;

	BOOL	bIntersect			= FALSE; //enable_scissor(L);
	{
		// setup xform
		L->xform_calc					();
		RCache.set_xform_world			(L->m_xform			);
		RCache.set_xform_view			(Device.mView		);
		RCache.set_xform_project		(Device.mProject	);
		bIntersect						= enable_scissor	(L);

		// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
		// *** thus can cope without stencil clear with 127 lights
		// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
		RCache.set_ColorWriteEnable		(FALSE);
		RCache.set_Element				(shader->E[0]);			// masker

		// backfaces: if (stencil>=1 && zfail)			stencil = light_id
		RCache.set_CullMode				(CULL_CW);
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		draw_volume						(L);

		// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
		RCache.set_CullMode				(CULL_CCW);
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		draw_volume						(L);
	}

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	RCache.set_ColorWriteEnable				();
	if (bIntersect)	RCache.set_CullMode		(CULL_CW);		// back
	else			RCache.set_CullMode		(CULL_CCW);		// front

	// 2D texgen (texture adjustment matrix)
	Fmatrix			m_Texgen;
	{
		float	_w						= float(Device.dwWidth);
		float	_h						= float(Device.dwHeight);
		float	o_w						= (.5f / _w);
		float	o_h						= (.5f / _h);
		Fmatrix			m_TexelAdjust		= 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
		};
		m_Texgen.mul	(m_TexelAdjust,RCache.xforms.m_wvp);
	}

	// Shadow xform (+texture adjustment matrix)
	Fmatrix			m_Shadow,m_Lmap;
	{
		float			fTexelOffs			= (.5f / DSM_size);
		float			view_dim			= float(RImplementation.LR.S_size)/float(DSM_size);
		float			view_sx				= float(RImplementation.LR.S_posX)/float(DSM_size);
		float			view_sy				= float(RImplementation.LR.S_posY)/float(DSM_size);
		float			fRange				= float(1.f);
		float			fBias				= -0.0002f*fRange;
		Fmatrix			m_TexelAdjust		= {
			view_dim/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim/2.f + view_sx + fTexelOffs,	view_dim/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		Fmatrix			xf_world;		xf_world.invert	(Device.mView);
		Fmatrix			xf_view			= RImplementation.LR.S_view;
		Fmatrix			xf_project;		xf_project.mul	(m_TexelAdjust,RImplementation.LR.S_project);
		m_Shadow.mul					(xf_view, xf_world);
		m_Shadow.mulA					(xf_project	);

		// lmap
						view_dim			= 1.f;
						view_sx				= 0.f;
						view_sy				= 0.f;
		Fmatrix			m_TexelAdjust2		= {
			view_dim/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim/2.f + view_sx + fTexelOffs,	view_dim/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		xf_project.mul		(m_TexelAdjust2,RImplementation.LR.S_project);
		m_Lmap.mul			(xf_view, xf_world);
		m_Lmap.mulA			(xf_project	);
	}

	// Common constants
	Fvector		L_dir,L_clr,L_pos;	float L_spec;
	L_clr.set					(L->color.r,L->color.g,L->color.b);
	L_clr.div					(ps_r2_ls_dynamic_range);
	L_spec						= L_clr.magnitude()/_sqrt(3.f);
	Device.mView.transform_tiny	(L_pos,L->position);
	Device.mView.transform_dir	(L_dir,L->direction);
	L_dir.normalize				();

	// Draw volume with projective texgen
	// CHK_DX	(Q->Issue(D3DISSUE_BEGIN));
	{
		// Lighting
		RCache.set_Element			(shader->E[ L->flags.bShadow ? 1:2 ]);

		// Constants
		RCache.set_c				("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,1/(L->range*L->range));
		RCache.set_c				("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
		RCache.set_c				("m_texgen",		m_Texgen	);
		RCache.set_c				("m_shadow",		m_Shadow	);
		RCache.set_c				("m_lmap",			m_Lmap		);

		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
		draw_volume					(L);
	}
	// CHK_DX	(Q->Issue	(D3DISSUE_END));
	// DWORD	pixels	= 0;
	// CHK_DX	(Q->GetData	(&pixels,sizeof(pixels),D3DGETDATA_FLUSH));
	//Msg		("%8X : fragments(%d), size(%d)",u32(L),pixels,RImplementation.LR.S_size);

	dwLightMarkerID					+=	2;	// keep lowest bit always setted up
	CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));
}

/*
		// Shader + constants
		float circle				= ps_r2_ls_ssm_kernel / RImplementation.LR.S_size;
		Fvector4 J; float scale		= circle/11.f;
		R_constant* _C				= RCache.get_c			("J_spot");
		if (_C)		{
			// Radial jitter (12 samples)
			J.set(11, 0,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,0,J.x,J.y,-J.y,-J.x);
			J.set(19, 3,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,1,J.x,J.y,-J.y,-J.x);
			J.set(22, 11, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,2,J.x,J.y,-J.y,-J.x);
			J.set(19, 19, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,3,J.x,J.y,-J.y,-J.x);
			J.set(9,  7,  15, 9);	J.sub(11); J.mul(scale);	RCache.set_ca	(_C,4,J.x,J.y, J.w, J.z);
			J.set(13, 15, 7,  13);	J.sub(11); J.mul(scale);	RCache.set_ca	(_C,5,J.x,J.y, J.w, J.z);
		}

*/

#include "stdafx.h"

extern Fvector du_cone_vertices[DU_CONE_NUMVERTEX];

void CRenderTarget::accum_spot_shadow	(light* L)
{
	// *** assume accumulator setted up ***
	// *****************************	Mask by stencil		*************************************
	Shader*		shader			= L->s_spot_s;
	if (0==shader)				shader = s_accum_spot_s;

	if (1)
	{
		// scale to account range and angle
		float		s			= 2.f*L->range*tanf(L->cone/2.f);	
		Fmatrix		mScale;		mScale.scale(s,s,L->range);		// make range and radius
		
		// build final rotation / translation
		Fvector					L_dir,L_up,L_right;
		L_dir.set				(L->direction);			L_dir.normalize		();
		L_up.set				(0,1,0);				if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
		L_right.crossproduct	(L_up,L_dir);			L_right.normalize	();
		L_up.crossproduct		(L_dir,L_right);		L_up.normalize		();

		Fmatrix		mR;
		mR.i					= L_right;		mR._14		= 0;
		mR.j					= L_up;			mR._24		= 0;
		mR.k					= L_dir;		mR._34		= 0;
		mR.c					= L->position;	mR._44		= 1;

		// final xform
		Fmatrix		xf;			xf.mul			(mR,mScale);

		// setup xform
		RCache.set_xform_world			(xf					);
		RCache.set_xform_view			(Device.mView		);
		RCache.set_xform_project		(Device.mProject	);

		// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
		// *** thus can cope without stencil clear with 127 lights
		// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
		CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	0	));

		RCache.set_Element				(shader->E[0]);			// masker
		RCache.set_Geometry				(g_accum_spot);

		// backfaces: if (stencil>=1 && zfail)	stencil = light_id
		CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CW			));
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_CONE_NUMVERTEX,0,DU_CONE_NUMFACES);

		// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
		CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			));
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_CONE_NUMVERTEX,0,DU_CONE_NUMFACES);

		CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ));
	}

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	{
		Fmatrix& M						= RCache.xforms.m_wvp;
		BOOL	bIntersect				= FALSE;
		for (u32 vit=0; vit<DU_CONE_NUMVERTEX; vit++)
		{
			Fvector&	v	= du_cone_vertices[vit];
			float _z = v.x*M._13 + v.y*M._23 + v.z*M._33 + M._43;
			float _w = v.x*M._14 + v.y*M._24 + v.z*M._34 + M._44;
			if (_z<=0 || _w<=0)
			{
				bIntersect	= TRUE;
				break;
			}
		}
		if (bIntersect)	{CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,	D3DCULL_CW	));} 	// back
		else			{CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,	D3DCULL_CCW	));} 	// front
	}

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
	Fmatrix			m_Shadow;
	{
		float			fTexelOffs			= (.5f / DSM_size);
		u32				uRange				= 1; 
		float			fRange				= float(uRange);
		float			fBias				= -0.0002f*fRange;
		Fmatrix			m_TexelAdjust		= 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		Fmatrix			xf_world;		xf_world.invert	(Device.mView);
		Fmatrix			xf_view			= RImplementation.LR.L_view;
		Fmatrix			xf_project;		xf_project.mul	(m_TexelAdjust,RImplementation.LR.L_project);
		m_Shadow.mul					(xf_view, xf_world);
		m_Shadow.mulA					(xf_project	);
	}

	// Common constants
	Fvector		L_dir,L_clr,L_pos;	float L_spec;
	L_clr.set					(L->color.r,L->color.g,L->color.b);
	L_clr.div					(ps_r2_ls_dynamic_range);
	L_spec						= L_clr.magnitude()/_sqrt(3.f);
	Device.mView.transform_tiny	(L_pos,L->position);
	Device.mView.transform_dir	(L_dir,L->direction);
	L_dir.normalize				();

	// Perform "unmasking" where dot(L,N) < 0
	if (ps_r2_ls_flags.test(R2FLAG_SPOT_UNMASK))
	{
		// General: if stencil>=light_id && alpha<="small_value" => stencil=0x1
		// Unmasking (note: alpha-func assumed to be "greater" we need "less" here
		// Another: projective divide controlled by sampler-state in ps_1_1
		CHK_DX						(HW.pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_LESS		));
		CHK_DX						(HW.pDevice->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED));
		RCache.set_Element			(shader->E[1]);
		RCache.set_c				("light_direction",	-L_dir.x,-L_dir.y,-L_dir.z,0.f);
		RCache.set_c				("m_texgen",		m_Texgen);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x01,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.set_Geometry			(g_accum_spot);
		RCache.Render				(D3DPT_TRIANGLELIST,0,0,DU_CONE_NUMVERTEX,0,DU_CONE_NUMFACES);
		CHK_DX						(HW.pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER	));
	}

	// Draw volume with projective texgen
	{
		// Lighting
		RCache.set_Element			(shader->E[2]);

		// Constants
		RCache.set_c				("light_position",	L_pos.x,L_pos.y,L_pos.z,1/L->range);
		RCache.set_c				("light_direction",	L_dir.x,L_dir.y,L_dir.z,0.f);
		RCache.set_c				("light_color",		L_clr.x,L_clr.y,L_clr.z,L_spec);
		RCache.set_c				("m_texgen",		m_Texgen);
		RCache.set_c				("m_shadow",		m_Shadow);

		// Shader + constants
		float circle				= ps_r2_ls_ssm_kernel / DSM_size;
		Fvector4 J; float scale		= circle/11.f;
		R_constant* _C				= RCache.get_c			("jitter");
		if (_C)
		{
			// Radial jitter (12 samples)
			J.set(11, 0,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,0,J.x,J.y,-J.y,-J.x);
			J.set(19, 3,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,1,J.x,J.y,-J.y,-J.x);
			J.set(22, 11, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,2,J.x,J.y,-J.y,-J.x);
			J.set(19, 19, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,3,J.x,J.y,-J.y,-J.x);
			J.set(9,  7,  15, 9);	J.sub(11); J.mul(scale);	RCache.set_ca	(_C,4,J.x,J.y, J.w, J.z);
			J.set(13, 15, 7,  13);	J.sub(11); J.mul(scale);	RCache.set_ca	(_C,5,J.x,J.y, J.w, J.z);
		}

		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
		RCache.set_Geometry			(g_accum_spot);
		RCache.Render				(D3DPT_TRIANGLELIST,0,0,DU_CONE_NUMVERTEX,0,DU_CONE_NUMFACES);
	}

	dwLightMarkerID					+=	2;	// keep lowest bit always setted up
}

void CRenderTarget::accum_spot_unshadow	(light* L)
{
#pragma todo("Have to implement unshadowed spot-light")
}

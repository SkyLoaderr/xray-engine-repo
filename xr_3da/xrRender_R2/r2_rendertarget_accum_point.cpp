#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	if (RImplementation.b_nv3x)		{
		Msg	("! nv3x code path: shadowed point sources not implemented");
		return;
	}

	// Common
	Fvector		L_pos;
	float		L_spec;
	float		L_R					= L->range;
	Fvector		L_clr;				L_clr.set(L->color.r,L->color.g,L->color.b);
	L_clr.div						(ps_r2_ls_dynamic_range);
	L_spec							= L_clr.magnitude()/_sqrt(3.f);
	Device.mView.transform_tiny		(L_pos,L->position);

	// Xforms
	Fmatrix		mW;
	mW.scale						(L_R,L_R,L_R);
	mW.translate_over				(L->position);
	RCache.set_xform_world			(mW);
	RCache.set_xform_view			(Device.mView);
	RCache.set_xform_project		(Device.mProject);

	// *****************************	Mask by stencil		*************************************
	// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
	// *** thus can cope without stencil clear with 127 lights
	// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
	RCache.set_Element				(s_accum_point_s->E[0]);			// masker
	RCache.set_Geometry				(g_accum_point);
	RCache.set_ColorWriteEnable		(FALSE);

	// backfaces: if (stencil>=1 && zfail)	stencil = light_id
	RCache.set_CullMode				(CULL_CW);
	RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
	RCache.set_CullMode				(CULL_CCW);
	RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
	RCache.set_ColorWriteEnable		();

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	Fmatrix& M						= Device.mFullTransform;
	Fvector4 plane;
	plane.x							= -(M._14 + M._13);
	plane.y							= -(M._24 + M._23);
	plane.z							= -(M._34 + M._33);
	plane.w							= -(M._44 + M._43);
	float denom						= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
	plane.mul						(denom);
	Fplane	P;	P.n.set(plane.x,plane.y,plane.z); P.d = plane.w;
	float	p_dist					= P.classify	(L->position) - L->range;
	if (p_dist<0)					{
		RCache.set_Element			(s_accum_point_s->E[2]);	// back
		RCache.set_CullMode			(CULL_CW);
	}
	else							{
		RCache.set_Element			(s_accum_point_s->E[1]);	// front
		RCache.set_CullMode			(CULL_CCW);
	}

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

	// Constants
	RCache.set_c					("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,1/(L_R*L_R));
	RCache.set_c					("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
	RCache.set_c					("m_tex",			m_Tex);
	R_constant* _C					= RCache.get_c		("J_point");
	if (_C)
	{
		Fvector4	J;
		float		scale			= ps_r2_ls_psm_kernel/100.f;
		J.set(-1,-1,-1); J.mul(scale); RCache.set_ca	(_C,0,J);
		J.set(+1,-1,-1); J.mul(scale); RCache.set_ca	(_C,1,J);
		J.set(-1,-1,+1); J.mul(scale); RCache.set_ca	(_C,2,J);
		J.set(+1,-1,+1); J.mul(scale); RCache.set_ca	(_C,3,J);
		J.set(-1,+1,-1); J.mul(scale); RCache.set_ca	(_C,4,J);
		J.set(+1,+1,-1); J.mul(scale); RCache.set_ca	(_C,5,J);
		J.set(-1,+1,+1); J.mul(scale); RCache.set_ca	(_C,6,J);
		J.set(+1,+1,+1); J.mul(scale); RCache.set_ca	(_C,7,J);
	}

	// Render if (stencil >= light_id && z-pass)
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
	RCache.set_Geometry					(g_accum_point);
	RCache.Render						(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
	dwLightMarkerID						+=	2;	// keep lowest bit always setted up
}

void CRenderTarget::accum_point_unshadow(light* L)
{
	// Common
	Fvector		L_pos;
	float		L_spec;
	float		L_R					= L->range;
	Fvector		L_clr;				L_clr.set(L->color.r,L->color.g,L->color.b);
	L_clr.div						(ps_r2_ls_dynamic_range);
	L_spec							= L_clr.magnitude()/_sqrt(3.f);
	Device.mView.transform_tiny		(L_pos,L->position);

	// Xforms
	Fmatrix mW;
	mW.scale						(L_R,L_R,L_R);
	mW.translate_over				(L->position);
	RCache.set_xform_world			(mW);
	RCache.set_xform_view			(Device.mView);
	RCache.set_xform_project		(Device.mProject);

	// *****************************	Mask by stencil		*************************************
	// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
	// *** thus can cope without stencil clear with 127 lights
	// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
	RCache.set_Element				(s_accum_point_uns->E[0]);	// masker
	RCache.set_Geometry				(g_accum_point);
	RCache.set_ColorWriteEnable		(FALSE);

	// backfaces: if (stencil>=1 && zfail)	stencil = light_id
	RCache.set_CullMode				(CULL_CW);
	RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
	RCache.set_CullMode				(CULL_CCW);
	RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
	RCache.set_ColorWriteEnable		();

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	Fmatrix& M						= Device.mFullTransform;
	Fvector4 plane;
	plane.x							= -(M._14 + M._13);
	plane.y							= -(M._24 + M._23);
	plane.z							= -(M._34 + M._33);
	plane.w							= -(M._44 + M._43);
	float denom						= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
	plane.mul						(denom);
	Fplane	P;	P.n.set(plane.x,plane.y,plane.z); P.d = plane.w;
	float	p_dist					= P.classify	(L->position) - L->range;
	if (p_dist<0)					{
		RCache.set_Element			(s_accum_point_uns->E[2]);	// back
		RCache.set_CullMode			(CULL_CW);
	}
	else							{
		RCache.set_Element			(s_accum_point_uns->E[1]);	// front
		RCache.set_CullMode			(CULL_CCW);
	}

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

	// Constants
	RCache.set_c						("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,1/(L_R*L_R));
	RCache.set_c						("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
	RCache.set_c						("m_tex",			m_Tex);

	// Render if (stencil >= light_id && z-pass)
	// RCache.set_Stencil		(FALSE);
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
	RCache.set_Geometry					(g_accum_point);
	RCache.Render						(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
	dwLightMarkerID						+=	2;	// keep lowest bit always setted up
}

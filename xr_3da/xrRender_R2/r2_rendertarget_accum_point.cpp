#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	// Common
	Fvector		L_pos;
	float		L_R					= L->sphere.R;
	Fcolor		L_clr				= L->color;
	float		np					= VIEWPORT_NEAR+EPS;
	Device.mView.transform_tiny		(L_pos,L->sphere.P);

	// Xforms
	Fmatrix mW;
	mW.scale						(L_R,L_R,L_R);
	mW.translate_over				(L->sphere.P);
	RCache.set_xform_world			(mW);
	RCache.set_xform_view			(Device.mView);
	RCache.set_xform_project		(Device.mProject);

	// *****************************	Mask by stencil		*************************************
	// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
	// *** thus can cope without stencil clear with 127 lights
	// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
	RCache.set_Element				(s_accum_point->E[0]);			// masker
	RCache.set_Geometry				(g_accum_point);
	CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	0	));
	CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));

	// backfaces: if (stencil>=1 && zfail)	stencil = light_id
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CW			));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			dwLightMarkerID		));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0x01				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_REPLACE));
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
	/*
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x01				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_REPLACE));
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
	*/

	CHK_DX							(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ));

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
	float	p_dist					= P.classify	(L->sphere.P) - L->sphere.R;
	if (p_dist<0)					{
		RCache.set_Element(s_accum_point->E[2]);	// back
		CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,	D3DCULL_CW		)); 	
	}
	else							{
		RCache.set_Element(s_accum_point->E[1]);	// front
		CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,	D3DCULL_CCW		)); 	
	}

	// 2D texgen (texture adjustment matrix)
	Fvector2						p0,p1;
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
	Fmatrix			m_TexGen;
	m_TexGen.mul	(m_TexelAdjust,Device.mFullTransform);

	// Constants
	RCache.set_c					("light_position",	L_pos.x,L_pos.y,L_pos.z,1/L_R);
	RCache.set_c					("light_color",		L_clr.r,L_clr.g,L_clr.b,.15f*L_clr.magnitude_rgb());
	RCache.set_c					("m_tex",			m_TexGen);
	R_constant* _C					= RCache.get_c		("jitter");
	if (_C)
	{
		Fvector4	J;
		float		scale			= 1.f/50.f;
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
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			dwLightMarkerID		));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0x00				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));

	RCache.set_Geometry				(g_accum_point);
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	//
	dwLightMarkerID					+=	2;	// keep lowest bit always setted up
}

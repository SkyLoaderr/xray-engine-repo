#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	// Common
	Fvector		L_pos;
	float		L_spec;
	float		L_R					= L->sphere.R;
	Fvector		L_clr;				L_clr.set(L->color.r,L->color.g,L->color.b);
	L_clr.div						(ps_r2_ls_dynamic_range);
	L_spec							= L_clr.magnitude()/_sqrt(3.f);
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
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x01				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_REPLACE));
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

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

	// Test (bias+scale)
	Fmatrix			m_T;	m_T.translate	(-0.5f, -0.5f, -0.5f);
	Fmatrix			m_S;	m_S.scale		( 2.0f,  2.0f,  2.0f);
	Fmatrix			m_1;	m_1.mul			(m_S,m_T);

							m_T.translate	(-1.0f, -1.0f, -1.0f);
							m_S.scale		( 2.0f,  2.0f,  2.0f);
	Fmatrix			m_2;	m_2.mul			(m_T,m_S);

	// 2D texgen (texture adjustment matrix)
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
	Fmatrix			m_Tex;
	m_Tex.mul		(m_TexelAdjust,RCache.xforms.m_wvp);

	// Constants
	RCache.set_c					("light_position",	L_pos.x,L_pos.y,L_pos.z,1/L_R);
	RCache.set_c					("light_color",		L_clr.x,L_clr.y,L_clr.z,L_spec);
	RCache.set_c					("m_tex",			m_Tex);
	R_constant* _C					= RCache.get_c		("jitter");
	if (_C)
	{
		Fvector4	J;
		float		scale			= 1.f/100.f;
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
	dwLightMarkerID						+=	2;	// keep lowest bit always setted up

	RCache.set_Geometry					(g_accum_point);
	RCache.Render						(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	// XForm vertices + manual, non projective TC gen
	/*
	Fvector2	p0,p1;	
	u32			Offset;
	u32			C						= D3DCOLOR_RGBA	(255,255,255,255);
	float		d_Z						= EPS_S;
	float		d_W						= 1.f;
	FVF::TL* pv							= (FVF::TL*) RCache.Vertex.Lock	(4,g_accum_point_tl->vb_stride,Offset);
	for (u32 vert=0; vert<DU_SPHERE_NUMVERTEX; vert++)
	{
		Fvector4	P;
		RCache.xforms.m_wvp.transform	()
	}
	*/
	// Draw full-screen quad textured with our scene image
	/*
	p1.set							((_w+.5f)/_w, (_h+.5f)/_h );
	FVF::TL* pv						= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set							(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set							(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set							(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set							(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock			(4,g_combine->vb_stride);
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,	D3DCULL_CCW		)); 	
	RCache.set_Geometry				(g_combine);
	RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	*/
}

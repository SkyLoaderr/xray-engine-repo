#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	// Common
	Fvector		L_pos;
	float		L_R					= L->sphere.R;
	Fcolor		L_clr				= L->color;
	float		np					= VIEWPORT_NEAR+EPS;
	Device.mView.transform_tiny		(L_pos,L->sphere.P);

	// Near plane for fog
	Fmatrix& M						= Device.mFullTransform;
	Fvector4 plane;
	plane.x							= -(M._14 + M._13);
	plane.y							= -(M._24 + M._23);
	plane.z							= -(M._34 + M._33);
	plane.w							= -(M._44 + M._43);
	float denom						= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
	plane.mul						(denom);

	// Xforms
	Fmatrix mW;
	mW.scale						(L_R,L_R,L_R);
	mW.translate_over				(L->sphere.P);
	RCache.set_xform_world			(mW);
	RCache.set_xform_view			(Device.mView);
	RCache.set_xform_project		(Device.mProject);

	// Select shader (front or back-faces), *** back, if intersect near plane
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

	// 2D texgen
	Fvector2						p0,p1;
	float	_w						= float(Device.dwWidth);
	float	_h						= float(Device.dwHeight);
	p0.set							(.5f/_w, .5f/_h);
	p1.set							((_w+.5f)/_w, (_h+.5f)/_h );

	// Constants
	RCache.set_c					("light_position",	L_pos.x,L_pos.y,L_pos.z,1/L_R);
	RCache.set_c					("light_color",		L_clr.r,L_clr.g,L_clr.b,.15f*L_clr.magnitude_rgb());
	RCache.set_c					("near",			np,np,np,np);
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

	// Render if stencil >= 0x1
	RCache.set_Geometry				(g_accum_point);
	RCache.Render					(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	//
	dwLightMarkerID					+=	2;	// keep lowest bit always setted up
}

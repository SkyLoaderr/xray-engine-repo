#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	// Xforms
	{
		Fmatrix mW;
		mW.scale					(L_R,L_R,L_R);
		mW.translate_over			(L->sphere.P);
		RCache.set_xform_world		(mW);
		RCache.set_xform_view		(Device.mView);
		RCache.set_xform_project	(Device.mProject);
	}

	// Mask area by stencil
	Fvector2					p0,p1;
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Constants
	RCache.set_Shader			(s_accum_point);
	Fvector4	J;
	Fvector		L_pos;
	float		L_R				= L->sphere.R;
	Fcolor		L_clr			= L->color;
	float		np				= VIEWPORT_NEAR+EPS;
	float		scale			= 1.f/50.f;
	Device.mView.transform_tiny	(L_pos,L->sphere.P);
	RCache.set_c				("light_position",	L_pos.x,L_pos.y,L_pos.z,1/L_R);
	RCache.set_c				("light_color",		L_clr.r,L_clr.g,L_clr.b,.15f*L_clr.magnitude_rgb());
	RCache.set_c				("near",			np,np,np,np);
	R_constant* _C				= RCache.get_c		("jitter");
	if (_C)
	{
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
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,	D3DCULL_CCW		)); 	
	RCache.set_Geometry			(g_accum_point);
	RCache.Render				(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);

	//
	dwLightMarkerID				+=	2;	// keep lowest bit always setted up
}

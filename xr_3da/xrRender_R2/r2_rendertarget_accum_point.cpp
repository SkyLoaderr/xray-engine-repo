#include "stdafx.h"

void CRenderTarget::accum_point_shadow	(light* L)
{
	Fvector2					p0,p1;
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Constants
	Fvector4	J;
	Fvector		L_pos;
	float		L_R				= 10; //1/L->sphere.R;
	Fcolor		L_clr			= L->color;
	float		np				= .2f;
	float		scale			= 1.f/50.f;
	Device.mView.transform_tiny	(L_pos,L->sphere.P);
	RCache.set_c				("light_position",	L_pos.x,L_pos.y,L_pos.z,L_R);
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

	// Xforms

	// Render if stencil >= 0x1
	RCache.set_Geometry			(g_accum_point);
	RCache.set_Shader			(s_accum_point);
	RCache.Render				(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
}

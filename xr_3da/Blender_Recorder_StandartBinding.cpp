#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

#include "igame_level.h"
#include "environment.h"

// matrices
#define	BIND_DECLARE(xf)	\
class cl_xform_##xf	: public R_constant_setup {	virtual void setup (R_constant* C) { RCache.xforms.set_c_##xf (C); } }; \
	static cl_xform_##xf	binder_##xf
BIND_DECLARE(w);
BIND_DECLARE(v);
BIND_DECLARE(p);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

// fog
class cl_fog_plane	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C)
	{
		if (marker!=Device.dwFrame)
		{
			Fvector4		plane;
			Fmatrix&	M	= Device.mFullTransform;
			plane.x			= -(M._14 + M._13);
			plane.y			= -(M._24 + M._23);
			plane.z			= -(M._34 + M._33);
			plane.w			= -(M._44 + M._43);
			float denom		= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
			result.set		(plane.x*denom,	plane.y*denom,	plane.z*denom,	plane.w*denom);								// view-plane
		}
		RCache.set_c	(C,result);
	}
};
static cl_fog_plane		binder_fog_plane;

class cl_fog_params	: public R_constant_setup {
	u32			marker;
	float		f_near,f_far;
	virtual void setup(R_constant* C)
	{
		if (marker!=Device.dwFrame)
		{
			f_near	= g_pGameLevel->Environment->c_FogNear;
			f_far	= 1/(g_pGameLevel->Environment->c_FogFar - f_near);
		}
		RCache.set_c	(C,f_near,f_far,0,0);
	}
};
static cl_fog_params	binder_fog_params;

// eye-params
class cl_eye_P		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector&		V	= Device.vCameraPosition;
		RCache.set_c	(C,V.x,V.y,V.z,1);
	}
};
static cl_eye_P		binder_eye_P;

// eye-params
class cl_eye_D		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector&		V	= Device.vCameraDirection;
		RCache.set_c	(C,V.x,V.y,V.z,0);
	}
};
static cl_eye_D		binder_eye_D;

// eye-params
class cl_eye_N		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector&		V	= Device.vCameraTop;
		RCache.set_c	(C,V.x,V.y,V.z,0);
	}
};
static cl_eye_N		binder_eye_N;

// Standart constant-binding
void	CBlender_Compile::SetMapping	()
{
	// matrices
	r_Constant				("m_W",			&binder_w);
	r_Constant				("m_V",			&binder_v);
	r_Constant				("m_P",			&binder_p);
	r_Constant				("m_WV",		&binder_wv);
	r_Constant				("m_VP",		&binder_vp);
	r_Constant				("m_WVP",		&binder_wvp);

	// fog-params
	r_Constant				("fog_plane",	&binder_fog_plane);
	r_Constant				("fog_params",	&binder_fog_params);

	// eye-params
	r_Constant				("eye_position",	&binder_eye_P);
	r_Constant				("eye_direction",	&binder_eye_D);
	r_Constant				("eye_normal",		&binder_eye_N);
}


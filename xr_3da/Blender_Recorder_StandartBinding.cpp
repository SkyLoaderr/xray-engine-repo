#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

#ifdef _EDITOR
	#include "ui_tools.h"
#else
	#include "igame_level.h"
	#include "environment.h"
	#include "xr_effsun.h"
#endif

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
#ifdef _EDITOR
			u32 fog_color;
            float s_fog, e_fog;
		    Tools.GetCurrentFog(fog_color, s_fog, e_fog);
#else
			f_near	= g_pGameLevel->Environment->c_FogNear;
			f_far	= 1/(g_pGameLevel->Environment->c_FogFar - f_near);
#endif
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

// D-Light0
class cl_sun0_color	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CSun&	sun				= *(g_pGameLevel->Environment->Suns.front());
			result.set				(sun.Color().r,	sun.Color().g,	sun.Color().b,	0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sun0_color		binder_sun0_color;

class cl_sun0_dir_w	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CSun&	sun				= *(g_pGameLevel->Environment->Suns.front());
			result.set				(sun.Direction().x,	sun.Direction().y,	sun.Direction().z,	0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sun0_dir_w		binder_sun0_dir_w;
class cl_sun0_dir_e	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			Fvector D;
			CSun&	sun				=	*(g_pGameLevel->Environment->Suns.front());
			Device.mView.transform_dir	(D,sun.Direction());
			D.normalize					();
			result.set					(D.x,D.y,D.z,0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sun0_dir_e		binder_sun0_dir_e;

// Standart constant-binding
void	CBlender_Compile::SetMapping	()
{
	// matrices
	r_Constant				("m_W",				&binder_w);
	r_Constant				("m_V",				&binder_v);
	r_Constant				("m_P",				&binder_p);
	r_Constant				("m_WV",			&binder_wv);
	r_Constant				("m_VP",			&binder_vp);
	r_Constant				("m_WVP",			&binder_wvp);

	// fog-params
	r_Constant				("fog_plane",		&binder_fog_plane);
	r_Constant				("fog_params",		&binder_fog_params);

	// eye-params
	r_Constant				("eye_position",	&binder_eye_P);
	r_Constant				("eye_direction",	&binder_eye_D);
	r_Constant				("eye_normal",		&binder_eye_N);

	// global-lighting (env params)
	r_Constant				("sun0_color",		&binder_sun0_color);
	r_Constant				("sun0_dir_w",		&binder_sun0_dir_w);
	r_Constant				("sun0_dir_e",		&binder_sun0_dir_e);

	// detail
	if (bDetail	&& detail_scaler)
		r_Constant			("dt_params",		detail_scaler);
}

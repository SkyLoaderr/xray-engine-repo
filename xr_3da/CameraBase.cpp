// CameraBase.cpp: implementation of the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CameraBase.h"
#include "xr_creator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraBase::CCameraBase(CObject* p, BOOL rlink)
{
	bRelativeLink	= rlink;
	vPosition.set	(0,0,0);
	vDirection.set	(0,0,1);
	vNormal.set		(0,1,0);
	yaw				= 0;
	pitch			= 0;
	roll			= 0;
	lim_yaw.set		(0.f,0.f);
	lim_pitch.set	(0.f,0.f);
	lim_roll.set	(0.f,0.f);
	bClampYaw		= false;
	bClampPitch		= false;
	bClampRoll		= false;
	parent			= p;	VERIFY(parent);
	f_fov			= 90;
}

CCameraBase::~CCameraBase()
{
}

IC float AClamp(Fvector2& l, float v){
	return (2*v-l[0]-l[1])/(l[1]-l[0]);
}

float CCameraBase::CheckLimYaw( ){
	if (bClampYaw){
		return AClamp(lim_yaw,yaw);
	}else 
		return 0;
}

float CCameraBase::CheckLimPitch( ){
	if (bClampYaw){
		return AClamp(lim_pitch,pitch);
	}else 
		return 0;
}

float CCameraBase::CheckLimRoll( ){
	if (bClampYaw){
		return AClamp(lim_roll,roll);
	}else 
		return 0;
}

#include "stdafx.h"
#pragma hdrstop

#include "..\GameFont.h"
#include "CameraLook.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraLook::CCameraLook(CObject* p, CInifile* ini, LPCSTR section, BOOL rlink ) : CCameraBase(p, rlink)
{
	style				= csLookAt;

	bApplyInert			= ini->r_bool		(section,"apply_inert");
	lim_yaw				= ini->r_fvector2	(section,"lim_yaw");
	lim_pitch			= ini->r_fvector2	(section,"lim_pitch");
	lim_zoom			= ini->r_fvector2	(section,"lim_zoom");
	rot_speed			= ini->r_fvector3	(section,"rot_speed");
	dist				= (lim_zoom[0]+lim_zoom[1])*0.5f;
	bClampPitch			= (lim_pitch[0]!=0)||(lim_pitch[1]!=0);
	bClampYaw			= (lim_yaw[0]!=0)||(lim_yaw[1]!=0);
	prev_d				= 0;
	if (bClampPitch)	pitch = (lim_pitch[0]+lim_pitch[1])*0.5f;
	if (bClampYaw)		yaw	  = (lim_yaw[0]+lim_yaw[1])*0.5f;
}

CCameraLook::~CCameraLook()
{
}

void CCameraLook::Update(Fvector& point, Fvector& noise_dangle)
{
	vPosition.set		(point);
	Fmatrix mR;
	mR.setHPB			(-yaw,-pitch,-roll);

	vDirection.set		(mR.k);
	vNormal.set			(mR.j);

	if (bRelativeLink){
		parent->clXFORM().transform_dir(vDirection);
		parent->clXFORM().transform_dir(vNormal);
	}

	Fvector				vDir;
	Collide::ray_query	R;

	float				covariance = VIEWPORT_NEAR*6.f;
	vDir.invert			(vDirection);
	parent->setEnabled	(false);
	g_pGameLevel->ObjectSpace.RayPick( point, vDir, dist+covariance, R);
	parent->setEnabled	(true);

	// �������������
	float d				= psCamSlideInert*prev_d+(1.f-psCamSlideInert)*(R.range-covariance);
	prev_d = d;
	
	vPosition.mul		(vDirection,-d-VIEWPORT_NEAR);
	vPosition.add		(point);
}

void CCameraLook::Move( int cmd, float val )
{
	switch (cmd){
	case kCAM_ZOOM_IN:	dist	-= val?val:(rot_speed.z*Device.fTimeDelta);	break;
	case kCAM_ZOOM_OUT:	dist	+= val?val:(rot_speed.z*Device.fTimeDelta);	break;
	case kDOWN:			pitch	-= val?val:(rot_speed.y*Device.fTimeDelta);	break;
	case kUP:			pitch	+= val?val:(rot_speed.y*Device.fTimeDelta);	break;
	case kLEFT:			yaw		-= val?val:(rot_speed.x*Device.fTimeDelta);	break;
	case kRIGHT:		yaw		+= val?val:(rot_speed.x*Device.fTimeDelta);	break;
	}
	if (bClampYaw)		clamp(yaw,lim_yaw[0],lim_yaw[1]);
	if (bClampPitch)	clamp(pitch,lim_pitch[0],lim_pitch[1]);
	clamp			(dist,lim_zoom[0],lim_zoom[1]);	
}

void CCameraLook::OnActivate( CCameraBase* old_cam )
{
	if (old_cam&&(bRelativeLink==old_cam->bRelativeLink))
	{
		yaw				= old_cam->yaw;
		vPosition.set	(old_cam->vPosition);
	}
	if (yaw>PI_MUL_2) yaw-=PI_MUL_2;
	if (yaw<-PI_MUL_2)yaw+=PI_MUL_2;
}

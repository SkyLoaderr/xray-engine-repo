#include "stdafx.h"
#include "Actor.h"
#include "..\CameraBase.h"

void CActor::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cam_Active();
	cam_active = style;
	old_cam->OnDeactivate();
	cam_Active()->OnActivate(old_cam);
}

void CActor::cam_Update(float dt, float fFOV)
{
	Fvector point, dangle;
	
	// get calc eye point 90% from player height
	Fvector						R;
	Movement.Box().getsize		(R);
	point.set					(0.f,.91f*R.y,0.f);
	svTransform.transform_tiny	(point);

	// soft crouch
	float dS = point.y-fPrevCamPos;
	if (_abs(dS)>EPS_L){
		point.y = 0.7f*fPrevCamPos+0.3f*point.y;
	}

	// save previous position of camera
	fPrevCamPos=point.y;

	// apply shift
	point.mad					(Device.vCameraDirection,cam_shift);

	// apply inertion
	switch (cam_active)
	{
	case eacFirstEye:
		dangle.set			(0,0,0);
		break;
	case eacLookAt: 
		dangle.set			(0,0,0);
		break;
	case eacFreeLook: 
		dangle.set			(0,0,0);
		break;
	}
	CCameraBase* C				= cameras	[cam_active];
	C->Update					(point,dangle);
	C->f_fov					= fFOV;
	pCreator->Cameras.Update	(C);

	// ::Render.Target.set_gray	(cam_gray);
}

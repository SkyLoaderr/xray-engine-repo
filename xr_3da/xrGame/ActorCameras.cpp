#include "stdafx.h"
#include "Actor.h"
#include "..\CameraBase.h"
#include "..\render.h"

void CActor::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cameras[cam_active];
	cam_active = style;
	old_cam->OnDeactivate();
	cameras[cam_active]->OnActivate(old_cam);
}

void CActor::cam_Update(float dt, BOOL bZoom)
{
	Fvector point, dangle;
	
	// get calc eye point 90% from player height
	Fvector						R;
	Movement.Box().getsize		(R);
	point.set					(0.f,.91f*R.y,0.f);
	svTransform.transform_tiny	(point);

	// soft crouch
	float dS = point.y-fPrevCamPos;
	if (fabsf(dS)>EPS_L){
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
	pCreator->Cameras.Update	(C);
	// ::Render.Target.set_gray	(cam_gray);
	//	C->f_fov					= bZoom?15.f:DEFAULT_FOV;
}

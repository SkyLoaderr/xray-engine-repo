#include "stdafx.h"
#include "Actor.h"
#include "..\CameraBase.h"

void CActor::ChangeCamStyle	(EActorCameras style){
	CCameraBase* old_cam = cameras[cam_active];
	cam_active = style;
	old_cam->OnDeactivate();
	cameras[cam_active]->OnActivate(old_cam);
	switch(cam_active){
	case eacFirstEye:	bVisible = false;	break;
	default:			bVisible = true;	break;
	}
}

void CActor::UpdateCamera(float dt)
{
	Fvector point, dangle;
	
	// get calc eye point 90% from player height
	Fvector						R;
	Movement.Box().getsize		(R);
	point.set					(0.f,R.y,0.f);
	svTransform.transform_tiny	(point);

	// soft crouch
	//-----------------------------------------------------------
	float dS = point.y-fPrevCamPos;
	if (fabsf(dS)>EPS_L){
		point.y = 0.8f*fPrevCamPos+0.2f*point.y;
	}

	// save previous position of camera
	fPrevCamPos=point.y;

	// apply inertion
	//-----------------------------------------------------------

	switch (cam_active)
	{
	case eacFirstEye:{
		// apply footstep bobbing effect
		Fvector offset;
		offset.set		(0,0,0);
		dangle.set		(0,0,0);
		cam_BobCycle	+= dt;
		
		if (mstate_real&mcAnyMove){
			if (isAccelerated(mstate_real)){
				offset.y	= 0.1f*fabsf(sinf(11*cam_BobCycle)); 
				dangle.x	= 0.01f*fabsf(cosf(cam_BobCycle*11));
				dangle.z	= 0.01f*cosf(cam_BobCycle*11);
				if (mstate_real&mcCrouch)
					dangle.y= 0.02f*sinf(cam_BobCycle*11);
			}else{
				offset.y	= 0.05f*fabsf(sinf(7*cam_BobCycle)); 
				dangle.x	= 0.01f*fabsf(cosf(cam_BobCycle*7));
				dangle.z	= 0.01f*cosf(cam_BobCycle*7);
				if (mstate_real&mcCrouch)
					dangle.y= 0.02f*sinf(cam_BobCycle*7);
			}
		}else{
			cam_BobCycle = 0;
		}
		point.add(offset);
		}
		break;
	case eacLookAt: 
		dangle.set		(0,0,0);
		break;
	case eacFreeLook: 
		dangle.set		(0,0,0);
		break;
	}
	CCameraBase* C				= cameras[cam_active];
	C->Update					(point,dangle);
	pCreator->Cameras.Update	(C);
}

#include "stdafx.h"
#include "Actor.h"
#include "../CameraBase.h"
#include "Car.h"
#include "ShootingHitEffector.h"

void CActor::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cam_Active();
	cam_active = style;
	old_cam->OnDeactivate();
	cam_Active()->OnActivate(old_cam);
}

void CActor::cam_Update(float dt, float fFOV)
{
	if(m_vehicle)
	{
		m_vehicle->cam_Update(dt);
		return;
	}
	
	Fvector point, dangle;
	
	// calc eye point from player height
	Fvector						R;
	m_PhysicMovementControl->Box().getsize		(R);
	point.set					(0.f,m_fCamHeightFactor*R.y,0.f);
	XFORM().transform_tiny	(point);

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
	if (Level().CurrentEntity() == this)
		g_pGameLevel->Cameras.Update	(C);

	// ::Render.Target.set_gray	(cam_gray);
}

void CActor::LoadShootingEffector()
{
	if(!m_pShootingEffector) 
		m_pShootingEffector = xr_new<SShootingEffector>();


	m_pShootingEffector->ppi.duality.h		= pSettings->r_float("shooting_effector","duality_h");
	m_pShootingEffector->ppi.duality.v		= pSettings->r_float("shooting_effector","duality_v");
	m_pShootingEffector->ppi.gray				= pSettings->r_float("shooting_effector","gray");
	m_pShootingEffector->ppi.blur				= pSettings->r_float("shooting_effector","blur");
	m_pShootingEffector->ppi.noise.intensity	= pSettings->r_float("shooting_effector","noise_intensity");
	m_pShootingEffector->ppi.noise.grain		= pSettings->r_float("shooting_effector","noise_grain");
	m_pShootingEffector->ppi.noise.fps		= pSettings->r_float("shooting_effector","noise_fps");

	sscanf(pSettings->r_string("shooting_effector","color_base"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_base.r, &m_pShootingEffector->ppi.color_base.g, &m_pShootingEffector->ppi.color_base.b);
	sscanf(pSettings->r_string("shooting_effector","color_gray"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_gray.r, &m_pShootingEffector->ppi.color_gray.g, &m_pShootingEffector->ppi.color_gray.b);
	sscanf(pSettings->r_string("shooting_effector","color_add"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_add.r,  &m_pShootingEffector->ppi.color_add.g,	&m_pShootingEffector->ppi.color_add.b);

	m_pShootingEffector->time				= pSettings->r_float("shooting_effector","time");
	m_pShootingEffector->time_attack		= pSettings->r_float("shooting_effector","time_attack");
	m_pShootingEffector->time_release		= pSettings->r_float("shooting_effector","time_release");

	m_pShootingEffector->ce_time			= pSettings->r_float("shooting_effector","ce_time");
	m_pShootingEffector->ce_amplitude		= pSettings->r_float("shooting_effector","ce_amplitude");
	m_pShootingEffector->ce_period_number	= pSettings->r_float("shooting_effector","ce_period_number");
	m_pShootingEffector->ce_power			= pSettings->r_float("shooting_effector","ce_power");
}

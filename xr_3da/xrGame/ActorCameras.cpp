#include "stdafx.h"
#include "Actor.h"
#include "../CameraBase.h"
#include "Car.h"

#include "Weapon.h"
#include "Inventory.h"

#include "ShootingHitEffector.h"
#include "SleepEffector.h"
#include "ActorEffector.h"

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
	XFORM().transform_tiny		(point);

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


	/*
	CWeapon *pWeapon = dynamic_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		dangle = pWeapon->GetRecoilDeltaAngle();
	}
	*/

	CCameraBase* C						= cam_Active();

	cameras[eacFirstEye]->Update		(point,dangle);
	cameras[eacFirstEye]->f_fov			= fFOV;
	EffectorManager().Update			(cameras[eacFirstEye]);

	if(eacFirstEye != cam_active)
	{
		C->Update					(point,dangle);
		C->f_fov					= fFOV;
	}

	if (Level().CurrentEntity() == this)
	{
		if(eacFirstEye == cam_active)
			EffectorManager().Apply		(C);

		Level().Cameras.Update	(C);
	}

		
	// ::Render.Target.set_gray	(cam_gray);
}

void CActor::LoadShootingEffector (LPCSTR section)
{
	if(!m_pShootingEffector) 
		m_pShootingEffector = xr_new<SShootingEffector>();


	m_pShootingEffector->ppi.duality.h		= pSettings->r_float(section,"duality_h");
	m_pShootingEffector->ppi.duality.v		= pSettings->r_float(section,"duality_v");
	m_pShootingEffector->ppi.gray				= pSettings->r_float(section,"gray");
	m_pShootingEffector->ppi.blur				= pSettings->r_float(section,"blur");
	m_pShootingEffector->ppi.noise.intensity	= pSettings->r_float(section,"noise_intensity");
	m_pShootingEffector->ppi.noise.grain		= pSettings->r_float(section,"noise_grain");
	m_pShootingEffector->ppi.noise.fps		= pSettings->r_float(section,"noise_fps");

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_base.r, &m_pShootingEffector->ppi.color_base.g, &m_pShootingEffector->ppi.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_gray.r, &m_pShootingEffector->ppi.color_gray.g, &m_pShootingEffector->ppi.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_add.r,  &m_pShootingEffector->ppi.color_add.g,	&m_pShootingEffector->ppi.color_add.b);

	m_pShootingEffector->time				= pSettings->r_float(section,"time");
	m_pShootingEffector->time_attack		= pSettings->r_float(section,"time_attack");
	m_pShootingEffector->time_release		= pSettings->r_float(section,"time_release");

	m_pShootingEffector->ce_time			= pSettings->r_float(section,"ce_time");
	m_pShootingEffector->ce_amplitude		= pSettings->r_float(section,"ce_amplitude");
	m_pShootingEffector->ce_period_number	= pSettings->r_float(section,"ce_period_number");
	m_pShootingEffector->ce_power			= pSettings->r_float(section,"ce_power");
}

void CActor::LoadSleepEffector	(LPCSTR section)
{
	if(!m_pSleepEffector) 
		m_pSleepEffector = xr_new<SSleepEffector>();

	m_pSleepEffector->ppi.duality.h			= pSettings->r_float(section,"duality_h");
	m_pSleepEffector->ppi.duality.v			= pSettings->r_float(section,"duality_v");
	m_pSleepEffector->ppi.gray				= pSettings->r_float(section,"gray");
	m_pSleepEffector->ppi.blur				= pSettings->r_float(section,"blur");
	m_pSleepEffector->ppi.noise.intensity	= pSettings->r_float(section,"noise_intensity");
	m_pSleepEffector->ppi.noise.grain		= pSettings->r_float(section,"noise_grain");
	m_pSleepEffector->ppi.noise.fps			= pSettings->r_float(section,"noise_fps");

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_base.r, &m_pSleepEffector->ppi.color_base.g, &m_pSleepEffector->ppi.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_gray.r, &m_pSleepEffector->ppi.color_gray.g, &m_pSleepEffector->ppi.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_add.r,  &m_pSleepEffector->ppi.color_add.g,  &m_pSleepEffector->ppi.color_add.b);

	m_pSleepEffector->time				= pSettings->r_float(section,"time");
	m_pSleepEffector->time_attack		= pSettings->r_float(section,"time_attack");
	m_pSleepEffector->time_release		= pSettings->r_float(section,"time_release");
}
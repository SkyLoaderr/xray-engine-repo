// EffectorZoomInertion.cpp: инерция(покачивания) оружия в режиме
//							 приближения
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorZoomInertion.h"


#define EFFECTOR_ZOOM_SECTION "zoom_inertion_effector"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorZoomInertion::CEffectorZoomInertion	() : CCameraEffector(eCEZoom,100000.f,TRUE)
{
	Load();
}

CEffectorZoomInertion::~CEffectorZoomInertion	()
{

}

void CEffectorZoomInertion::Load		()
{
	m_fCameraMoveEpsilon= pSettings->r_float(EFFECTOR_ZOOM_SECTION, "camera_move_epsilon");
	m_fDispMin			= pSettings->r_float(EFFECTOR_ZOOM_SECTION, "disp_min");
	m_fSpeedMin			= pSettings->r_float(EFFECTOR_ZOOM_SECTION, "speed_min");
	m_fZoomAimingDispK	= pSettings->r_float(EFFECTOR_ZOOM_SECTION, "zoom_aim_disp_k");
	m_fZoomAimingSpeedK	= pSettings->r_float(EFFECTOR_ZOOM_SECTION, "zoom_aim_speed_k");

	m_fFloatSpeed		= m_fSpeedMin;
	m_fDispRadius		= m_fDispMin;

	m_fEpsilon = 2*m_fFloatSpeed;


	m_vTargetVel.set(0.f,0.f,0.f);
	m_vCurrentPoint.set(0.f,0.f,0.f);
	m_vTargetPoint.set(0.f,0.f,0.f);

}

void CEffectorZoomInertion::SetParams	(float disp)
{
	float old_disp = m_fDispRadius;

	m_fDispRadius = disp*m_fZoomAimingDispK;
	if(m_fDispRadius<m_fDispMin) 
		m_fDispRadius = m_fDispMin;

	m_fFloatSpeed = disp*m_fZoomAimingSpeedK;
	if(m_fFloatSpeed<m_fSpeedMin) 
		m_fFloatSpeed = m_fSpeedMin;

	//для того, чтоб сразу прошел пересчет направления
	//движения прицела
	if(!fis_zero(old_disp-disp,EPS))
		m_fEpsilon = 2*disp;
}

BOOL CEffectorZoomInertion::Process		(Fvector &p, Fvector &d, Fvector &n, 
										 float& fFov, float& fFar, float& fAspect)
{
	bool camera_moved = false;

	//определяем двигал ли прицелом актер
	if(!d.similar(m_vOldCameraDir, m_fCameraMoveEpsilon))
		camera_moved = true;


	Fvector dir;
	dir.sub(m_vCurrentPoint,m_vTargetPoint);

	if(dir.magnitude()<m_fEpsilon)
	{
		m_fEpsilon = 2*m_fFloatSpeed;

		float half_disp_radius = m_fDispRadius/2.f;
		m_vTargetPoint.x = Random.randF(-half_disp_radius,half_disp_radius);
		m_vTargetPoint.y = Random.randF(-half_disp_radius,half_disp_radius);

		//Fvector norm = {0.f,0.f,1.f};
		//m_vTargetPoint.random_dir(norm, m_fDispRadius, Random);
		//m_vTargetPoint.z = 0.f;

		m_vTargetVel.sub(m_vTargetPoint, m_vCurrentPoint);
		m_vTargetVel.normalize_safe();
		m_vTargetVel.mul(m_fFloatSpeed);
	}

	m_vCurrentPoint.add(m_vTargetVel);

	m_vOldCameraDir = d;	

	if(!camera_moved)
		d.add(m_vCurrentPoint);

	return TRUE;
}
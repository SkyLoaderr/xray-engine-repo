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
	SetRndSeed(timeGetTime());
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

	m_dwDeltaTime		= pSettings->r_u32(EFFECTOR_ZOOM_SECTION, "delta_time");
	m_dwTimePassed		= 0;

	m_fFloatSpeed		= m_fSpeedMin;
	m_fDispRadius		= m_fDispMin;

	m_fEpsilon = 2*m_fFloatSpeed;


	m_vTargetVel.set(0.f,0.f,0.f);
	m_vCurrentPoint.set(0.f,0.f,0.f);
	m_vTargetPoint.set(0.f,0.f,0.f);
	m_vLastPoint.set(0.f,0.f,0.f);
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
	if(!fis_zero(old_disp-m_fDispRadius,EPS))
		m_fEpsilon = 2*m_fDispRadius;
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

	if(dir.magnitude()<m_fEpsilon || m_dwTimePassed>m_dwDeltaTime)
	{
		m_dwTimePassed = 0;
		
		m_fEpsilon = 2*m_fFloatSpeed;

		float half_disp_radius = m_fDispRadius/2.f;
		m_vTargetPoint.x = m_Random.randF(-half_disp_radius,half_disp_radius);
		m_vTargetPoint.y = m_Random.randF(-half_disp_radius,half_disp_radius);

		m_vTargetVel.sub(m_vTargetPoint, m_vCurrentPoint);

		m_vLastPoint.set(m_vCurrentPoint);
	}

	m_vCurrentPoint.lerp(m_vLastPoint, m_vTargetPoint, float(m_dwTimePassed)/m_dwDeltaTime);

	m_vOldCameraDir = d;	

	if(!camera_moved)
		d.add(m_vCurrentPoint);

	m_dwTimePassed += Device.dwTimeDelta;

	return TRUE;
}
// EffectorZoomInertion.h: инерция(покачивания) оружия в режиме 
//						   приближения	
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"

class CEffectorZoomInertion : public CCameraEffector
{
	//коэффициент скорости "покачивания" прицела
	float	m_fFloatSpeed;
	float	m_fDispRadius;

	float	m_fEpsilon;
	Fvector	m_vCurrentPoint;
	Fvector	m_vTargetPoint;
	Fvector	m_vTargetVel;

	Fvector m_vOldCameraDir;

	//параметры настройки эффектора
	float	m_fCameraMoveEpsilon;
	float	m_fDispMin;
	float	m_fSpeedMin;
	float	m_fZoomAimingDispK;
	float	m_fZoomAimingSpeedK;

public:
	CEffectorZoomInertion				();
	virtual ~CEffectorZoomInertion		();

			void	Load				();
			void	SetParams			(float disp);

	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d,
											Fvector &delta_n, float& fFov,
											float& fFar, float& fAspect);

};
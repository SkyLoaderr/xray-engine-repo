// EffectorShot.h: interface for the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"

class CEffectorShot : public CCameraEffector
{
	Fvector	vDispersionDir;
	float	fAngleCurrent;
	float	fRelaxSpeed;
	float	fMaxAngle;
	BOOL	bActive;
	float	fDispProbability;
	float	fHorzDispFactor;
public:
	CEffectorShot					(float max_angle, float relax_time , float max_angle_horz, float step_angle_horz);
	virtual ~CEffectorShot			();
	
	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);
			void	Shot				(float angle);
			void	MountedWeaponShot	();

			void	GetDeltaAngle	(Fvector& delta_angle);

protected:
			void	UpdateAngles	();
			float	fAngleHorz;
			float	fAngleHorzMax;
			float	fAngleHorzStep;


};
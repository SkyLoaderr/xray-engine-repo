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
public:
	CEffectorShot					(float max_angle, float relax_time);
	virtual ~CEffectorShot			();
	
	virtual	BOOL	Process			(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);
			void	Shot			(float angle);

			void	GetDeltaAngle	(Fvector& delta_angle);
};
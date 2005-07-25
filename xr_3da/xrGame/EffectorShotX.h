#pragma once

#include "EffectorShot.h"

class CEffectorShotX : public CEffectorShot
{	
	typedef CEffectorShot	inherited;
public:
	CEffectorShotX					(float max_angle, float relax_time , float max_angle_horz, float step_angle_horz);
	virtual ~CEffectorShotX			();

	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);
	virtual	void	GetDeltaAngle		(Fvector& delta_angle);
	virtual	void	Shot				(float angle);
	virtual	void	Clear				();
protected:
	virtual void	UpdateActorCamera	(float dPitch, float dYaw);
};
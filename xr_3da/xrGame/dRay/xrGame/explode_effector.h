#pragma once
#include "CameraEffector.h"


#define EXPLODE_EFFECTOR_TYPE_ID	7

class CExplodeEffector : public CCameraEffector {
	typedef CCameraEffector inherited;	

	float total;
	float max_amp;
	float period_number;
	float power;

public:
					CExplodeEffector		(float time, float amp, float periods, float power);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};

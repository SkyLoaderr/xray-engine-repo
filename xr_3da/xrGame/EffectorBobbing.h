#ifndef _EFFECTOR_BOBBING_H
#define _EFFECTOR_BOBBING_H
#pragma once

#include "..\effector.h"

class CEffectorBobbing : public CEffector  
{
	Fvector	vDirectionDiff;
	float	fTimeCurrent;
	float	fTimeTotal;
	float	fAngleCurrent;
	float	fAngleTotal;
public:
	CEffectorBobbing			(float relax_time, float angle);
	virtual ~CEffectorBobbing	();
	virtual	void	Process		(Fvector &p, Fvector &d, Fvector &n);

	void	Shot				();
};

#endif //_EFFECTOR_BOBBING_H

#ifndef _EFFECTOR_BOBBING_H
#define _EFFECTOR_BOBBING_H
#pragma once

#include "..\effector.h"

class CEffectorBobbing : public CEffector  
{
	float	fTime;
	Fvector	vAngleAmplitude;
	float	fYAmplitude;
	float	fSpeed;

	u32	dwMState;
	float	fReminderFactor;
public:
			CEffectorBobbing	();
	virtual ~CEffectorBobbing	();
	virtual	void	Process		(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n);
	void	SetState			(u32 st);
};

#endif //_EFFECTOR_BOBBING_H

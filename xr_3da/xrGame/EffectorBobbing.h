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

	DWORD	dwMState;
public:
			CEffectorBobbing	();
	virtual ~CEffectorBobbing	();
	virtual	void	Process		(Fvector &p, Fvector &d, Fvector &n);
	void	SetState			(DWORD st);
};

#endif //_EFFECTOR_BOBBING_H

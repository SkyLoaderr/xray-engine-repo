// EffectorShot.h: interface for the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"
#include "../cameramanager.h"
#include "Actor.h"

class CEffectorShot : public CCameraEffector
{
protected:
	float	fAngleCurrent;
	float	fRelaxSpeed;
	float	fMaxAngle;
	float	fAngleFrac;
	BOOL	bActive;
	float	fDispProbability;
	float	fHorzDispFactor;

	CActor*	m_pActor;

	CRandom		m_Random;
	s32			m_LastSeed;
public:
	CEffectorShot					(float max_angle, float relax_time , float max_angle_horz, float step_angle_horz, float angle_frac = 0.7f);
	virtual ~CEffectorShot			();
	
	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);
	virtual	void	Shot				(float angle);

	virtual void	SetActor			(CActor* pActor) {m_pActor = pActor;};
	virtual	void	Clear				() {};
	virtual	void	GetDeltaAngle		(Fvector& delta_angle);
	virtual	void	SetRndSeed			(s32 Seed);
	virtual CEffectorShot			*cast_effector_shot				()	{return this;}
protected:
			float	fAngleHorz;
			float	fAngleHorzMax;
			float	fAngleHorzStep;
};
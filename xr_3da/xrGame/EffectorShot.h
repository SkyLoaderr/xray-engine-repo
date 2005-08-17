// EffectorShot.h: interface for the CCameraShotEffector class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"
#include "../cameramanager.h"
#include "Actor.h"

class CWeaponShotEffector{
protected:
	float			fAngleVert;
	float			fAngleVertMax;
	float			fAngleVertFrac;
	float			fAngleHorz;
	float			fAngleHorzMax;
	float			fAngleHorzStep;
	float			fRelaxSpeed;
protected:
	BOOL			bActive;
private:
	CRandom			m_Random;
	s32				m_LastSeed;
public:
					CWeaponShotEffector	();
	virtual			~CWeaponShotEffector(){};

	void			Initialize			(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac);
	IC BOOL			IsActive			(){return bActive;}
	void			Update				();
	
	void			SetRndSeed			(s32 Seed);

	virtual void	Shot				(float angle);
	virtual void	GetDeltaAngle		(Fvector& delta_angle);
};

class CCameraShotEffector : public CWeaponShotEffector, public CCameraEffector
{
protected:
	CActor*			m_pActor;
public:
					CCameraShotEffector	(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac);
	virtual			~CCameraShotEffector();
	
	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);

	virtual void	SetActor			(CActor* pActor) {m_pActor = pActor;};
	virtual	void	Clear				() {};
	virtual CCameraShotEffector*		cast_effector_shot				()	{return this;}
};
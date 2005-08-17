// EffectorShot.cpp: implementation of the CCameraShotEffector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorShot.h"

//-----------------------------------------------------------------------------
// Weapon shot effector
//-----------------------------------------------------------------------------
CWeaponShotEffector::CWeaponShotEffector()
{
	fAngleHorz				= 0.f;
	fAngleVert				= -EPS_S;
	bActive					= FALSE;
	m_LastSeed				= 0;
	fRelaxSpeed				= 0.f;
	fAngleVertMax			= 0.f;
	fAngleVertFrac			= 1.f;
	fAngleHorzMax			= 0.f;
	fAngleHorzStep			= 0.f;
}

void CWeaponShotEffector::Initialize(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac)
{
	fRelaxSpeed				= _abs(relax_speed);
	fAngleVertMax			= _abs(max_angle);
	fAngleVertFrac			= _abs(angle_frac);
	fAngleHorzMax			= max_angle_horz;
	fAngleHorzStep			= step_angle_horz;
}

void CWeaponShotEffector::Shot	(float angle)
{
	fAngleVert				+= (angle*fAngleVertFrac+m_Random.randF(-1,1)*angle*(1-fAngleVertFrac));
	clamp					(fAngleVert,-fAngleVertMax,fAngleVertMax);
	if(fis_zero(fAngleVert-fAngleVertMax))
		fAngleVert			*= m_Random.randF(0.9f,1.1f);
	fAngleHorz				= fAngleHorz + (fAngleVert/fAngleVertMax)*m_Random.randF(-1,1)*fAngleHorzStep;
	clamp					(fAngleHorz,-fAngleHorzMax,fAngleHorzMax);
	bActive					= TRUE;
}

void CWeaponShotEffector::Update()
{
	if (bActive){
		float time_to_relax	= _abs(fAngleVert)/fRelaxSpeed;
		float relax_speed	= _abs(fAngleHorz)/time_to_relax;
		if (fAngleHorz>=0.f)
			fAngleHorz		-= relax_speed*Device.fTimeDelta;
		else
			fAngleHorz		+= relax_speed*Device.fTimeDelta;

		if (fAngleVert>=0.f){
			fAngleVert		-= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleVert<0.f) bActive	= FALSE;
		}else{
			fAngleVert		+= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleVert>0.f)	bActive	= FALSE;
		}
		if (!bActive){
			fAngleVert		= 0.f;
			fAngleHorz		= 0.f;
			m_LastSeed		= 0;
		}
	}
}

void CWeaponShotEffector::GetDeltaAngle	(Fvector &delta_angle)
{
	delta_angle.x			= -fAngleVert;
	delta_angle.y			= -fAngleHorz;
	delta_angle.z			= 0.0f;
}

void CWeaponShotEffector::SetRndSeed	(s32 Seed)
{
	if (m_LastSeed == 0){
		m_LastSeed			= Seed;
		m_Random.seed		(Seed);
	};
};

//-----------------------------------------------------------------------------
// Camera shot effector
//-----------------------------------------------------------------------------
CCameraShotEffector::CCameraShotEffector(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac) : CCameraEffector(eCEShot,100000.f,TRUE)
{
	CWeaponShotEffector::Initialize(max_angle, relax_speed, max_angle_horz, step_angle_horz, angle_frac);
	m_pActor		= NULL;
}

CCameraShotEffector::~CCameraShotEffector()
{
}

BOOL CCameraShotEffector::Process	(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	if (bActive){
		float		h,p;
		d.getHP		(h,p);
		d.setHP		(h+fAngleHorz,p+fAngleVert);
		Update		();
	}
	return TRUE;
}


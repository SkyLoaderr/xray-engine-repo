// EffectorShot.cpp: implementation of the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorShot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorShot::CEffectorShot	(float max_angle, float relax_speed, 
								 float disp_prob, float horz_factor) : CCameraEffector(eCEShot,100000.f,TRUE)
{
	fRelaxSpeed		= _abs(relax_speed);
	fAngleCurrent	= -EPS_S;
	fMaxAngle		= _abs(max_angle);
	bActive			= FALSE;

	fDispProbability = 1.f - disp_prob;
	fHorzDispFactor	 = horz_factor;
}

CEffectorShot::~CEffectorShot	()
{

}

void CEffectorShot::Shot		(float angle)
{
	fAngleCurrent	+= (angle*.75f+::Random.randF(-1,1)*angle*.25f);
	clamp			(fAngleCurrent,-fMaxAngle,fMaxAngle);
	float r			= (::Random.randF()>fDispProbability)?(fAngleCurrent/fMaxAngle)*fHorzDispFactor*::Random.randF(-1,1):0.f;
	vDispersionDir.set(r,1.f,r); 
	vDispersionDir.normalize_safe();
	bActive			= TRUE;
}


void CEffectorShot::MountedWeaponShot		()
{
	fAngleCurrent	= (fMaxAngle*.75f+::Random.randF(-1,1)*fMaxAngle*.25f);
	float r1		= (::Random.randF()>fDispProbability)?(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
	float r2		= (::Random.randF()>fDispProbability)?(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
	float r3		= (::Random.randF()>fDispProbability)?(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
	vDispersionDir.set(r1,r2,r3); 
	vDispersionDir.normalize_safe();
	bActive			= TRUE;
}

BOOL CEffectorShot::Process		(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	if (bActive){
		d.mad					(vDispersionDir,tanf(fAngleCurrent));
		if (fAngleCurrent>=0.f){
			fAngleCurrent		-= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleCurrent<0.f){
				fAngleCurrent	= 0.f;
				bActive			= FALSE;
			}
		}else{
			fAngleCurrent		+= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleCurrent>0.f){
				fAngleCurrent	= 0.f;
				bActive			= FALSE;
			}
		}
	}
	return TRUE;
}


void  CEffectorShot::GetDeltaAngle	(Fvector &delta_angle)
{
	delta_angle.x = -fAngleCurrent;
	delta_angle.y = 0.f;
	delta_angle.z = 0.f;
}
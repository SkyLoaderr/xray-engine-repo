// EffectorShot.cpp: implementation of the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorShot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorShot::CEffectorShot	(float max_angle, float relax_speed, 
								 float max_angle_horz, float step_angle_horz) : CCameraEffector(eCEShot,100000.f,TRUE)
{
	fRelaxSpeed		= _abs(relax_speed);
	fAngleCurrent	= -EPS_S;
	fMaxAngle		= _abs(max_angle);
	bActive			= FALSE;


	fAngleHorz = 0.f;
	fAngleHorzMax = max_angle_horz;//PI*0.05f;
	fAngleHorzStep = step_angle_horz;//PI*0.05f*0.2f;

}

CEffectorShot::~CEffectorShot	()
{

}


void CEffectorShot::Shot		(float angle)
{
	fAngleCurrent	+= (angle*.7f+::Random.randF(-1,1)*angle*.3f);
	clamp			(fAngleCurrent,-fMaxAngle,fMaxAngle);
	if(fis_zero(fAngleCurrent - fMaxAngle))
			fAngleCurrent *= ::Random.randF(0.9f,1.1f);

	fAngleHorz		= fAngleHorz + (fAngleCurrent/fMaxAngle)*::Random.randF(-1,1)*fAngleHorzStep;
	clamp			(fAngleHorz,-fAngleHorzMax,fAngleHorzMax);
		
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
	if (bActive)
	{
		UpdateAngles	();
		d.add					(vDispersionDir);

		float time_to_relax = _abs(fAngleCurrent)/fRelaxSpeed;
		float relax_speed = _abs(fAngleHorz)/time_to_relax;
		if (fAngleHorz>=0.f)
			fAngleHorz	 -= relax_speed*Device.fTimeDelta;
		else
			fAngleHorz	 += relax_speed*Device.fTimeDelta;



		if (fAngleCurrent>=0.f)
		{
			fAngleCurrent		-= fRelaxSpeed*Device.fTimeDelta;
		
			if (fAngleCurrent<0.f)
			{
				fAngleCurrent	= 0.f;
				fAngleHorz		= 0.f;
				bActive			= FALSE;
			}
		}
		else
		{
			fAngleCurrent		+= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleCurrent>0.f)
			{
				fAngleCurrent	= 0.f;
				fAngleHorz		= 0.f;
				bActive			= FALSE;
			}
		}
	}
	return TRUE;
}


void CEffectorShot::UpdateAngles	()
{
	vDispersionDir.x = 0.0f;
	vDispersionDir.y = tanf(fAngleCurrent);
	vDispersionDir.z = tanf(fAngleHorz);
}

void  CEffectorShot::GetDeltaAngle	(Fvector &delta_angle)
{
	delta_angle.x = -fAngleCurrent;
	delta_angle.y = 0.f;
	delta_angle.z = 0.f;
}
// EffectorShot.cpp: implementation of the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorShot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorShot::CEffectorShot	(float max_angle, float relax_speed,
								 float max_angle_horz, float step_angle_horz, float angle_frac) : CCameraEffector(eCEShot,100000.f,TRUE)
{
	fRelaxSpeed		= _abs(relax_speed);
	fAngleCurrent	= -EPS_S;
	fMaxAngle		= _abs(max_angle);
	fAngleFrac		= _abs(angle_frac);
	bActive			= FALSE;
	m_LastSeed = 0;


	fAngleHorz = 0.f;
	fAngleHorzMax = max_angle_horz;//PI*0.05f;
	fAngleHorzStep = step_angle_horz;//PI*0.05f*0.2f;

	m_pActor = NULL;

}

CEffectorShot::~CEffectorShot	()
{

}


void CEffectorShot::Shot		(float angle)
{
	fAngleCurrent	+= (angle*fAngleFrac+m_Random.randF(-1,1)*angle*(1-fAngleFrac));
	clamp			(fAngleCurrent,-fMaxAngle,fMaxAngle);
	if(fis_zero(fAngleCurrent - fMaxAngle))
			fAngleCurrent *= m_Random.randF(0.9f,1.1f);

	fAngleHorz		= fAngleHorz + (fAngleCurrent/fMaxAngle)*m_Random.randF(-1,1)*fAngleHorzStep;
	clamp			(fAngleHorz,-fAngleHorzMax,fAngleHorzMax);
		
	bActive			= TRUE;
}

BOOL CEffectorShot::Process		(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	if (bActive)
	{
#pragma todo("Andy to Andy: optimize next")
		float			h,p;
		d.getHP			(h,p);
		d.setHP			(h+fAngleHorz,p+fAngleCurrent);

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
				m_LastSeed		= 0;
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
				m_LastSeed		= 0;
			}
		}
	}
	return TRUE;
}

void  CEffectorShot::GetDeltaAngle	(Fvector &delta_angle)
{
	delta_angle.x = -fAngleCurrent;
	delta_angle.y = 0.0f;
	delta_angle.z = 0.0f;
}

void	CEffectorShot::SetRndSeed			(s32 Seed)
{
	if (m_LastSeed == 0)
	{
		m_LastSeed = Seed;
		m_Random.seed(Seed);
	};
};
// EffectorShot.cpp: implementation of the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorShot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorShot::CEffectorShot	(float max_angle, float relax_speed) : CEffector(cefShot,100000.f,TRUE)
{
	fRelaxSpeed		= relax_speed;
	fAngleCurrent	= -EPS_S;
	fMaxAngle		= max_angle;
}

CEffectorShot::~CEffectorShot	()
{

}

void CEffectorShot::Shot		(float angle)
{
	fAngleCurrent	+= (angle*.75f+::Random.randF(-1,1)*angle*.25f);
	clamp(fAngleCurrent,0.f,fMaxAngle);
	float r = (::Random.randF()>0.75f)?(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
	vDispersionDir.set(r,1.f,r); 
	vDispersionDir.normalize_safe();
}

BOOL CEffectorShot::Process		(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	if (fAngleCurrent<0) return TRUE;
	d.mad			(vDispersionDir,tanf(fAngleCurrent));
	fAngleCurrent	-= fRelaxSpeed*Device.fTimeDelta;
	return TRUE;
}

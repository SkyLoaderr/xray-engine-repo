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
	fRelaxSpeed		= _abs(relax_speed);
	fAngleCurrent	= -EPS_S;
	fMaxAngle		= _abs(max_angle);
	bActive			= FALSE;
}

CEffectorShot::~CEffectorShot	()
{

}

void CEffectorShot::Shot		(float angle)
{
	fAngleCurrent	+= (angle*.75f+::Random.randF(-1,1)*angle*.25f);
	clamp			(fAngleCurrent,-fMaxAngle,fMaxAngle);
	float r			= (::Random.randF()>0.75f)?(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
	vDispersionDir.set(r,1.f,r); 
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

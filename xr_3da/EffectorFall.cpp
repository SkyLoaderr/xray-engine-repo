// EffectorFall.cpp: implementation of the CEffectorFall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorFall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define FALL_SPEED 3.5f
#define FALL_MAXDIST 0.15f

CEffectorFall::CEffectorFall(float power,float life_time) : CEffector(cefFall, life_time)
{
	fPower	= (power>1)?1:((power<0)?0:power*power);
	fPhase	= 0;
}

CEffectorFall::~CEffectorFall()
{
}

void CEffectorFall::Process(Fvector &p, Fvector &d, Fvector &n)
{
	fPhase+=FALL_SPEED*Device.fTimeDelta;
	if (fPhase<1)	p.y-=FALL_MAXDIST*fPower*sinf(M_PI*fPhase+M_PI);
	else			fLifeTime=-1;
}

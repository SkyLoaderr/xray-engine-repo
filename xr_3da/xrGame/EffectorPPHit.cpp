#include "stdafx.h"
#include "EffectorPPHit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorPPHit::CEffectorPPHit(float time, float force) : CEffectorPP(cefppHit,time)
{
	fTime = fLifeTime = time;
}

CEffectorPPHit::~CEffectorPPHit	()
{
}

BOOL CEffectorPPHit::Process	(SPPInfo& pp)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;
	pp.duality.h	= 1.f * fLifeTime / fTime;
	pp.duality.v	= 1.f * fLifeTime / fTime;
	return TRUE;
}

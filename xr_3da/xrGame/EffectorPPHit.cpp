#include "stdafx.h"
#include "EffectorPPHit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorPPHit::CEffectorPPHit(float x, float y, float time, float force) : CEffectorPP(cefppHit,time)
{
	fTime = fLifeTime = time;
	m_x = x*force; m_y = y*force;
}

CEffectorPPHit::~CEffectorPPHit	()
{
}

BOOL CEffectorPPHit::Process	(SPPInfo& pp)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;
	pp.duality.h	= m_x * fLifeTime / fTime;
	pp.duality.v	= m_y * fLifeTime / fTime;
	return TRUE;
}

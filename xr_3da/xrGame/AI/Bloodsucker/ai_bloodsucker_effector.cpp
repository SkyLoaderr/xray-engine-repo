#include "stdafx.h"
#include "ai_bloodsucker_effector.h" 
#include "..\\..\\..\\effectorPP.h"

CBloodsuckerEffector::CBloodsuckerEffector(float time) 
					 : CEffectorPP(BLOODSUCKER_EFFECTOR_TYPE_ID, time)
{
	fLifeTime = time;
}

CBloodsuckerEffector::~CBloodsuckerEffector()
{

}

void CBloodsuckerEffector::Init ()
{
}

BOOL CBloodsuckerEffector::Process(SPPInfo& pp)
{
	inherited::Process();

	pp.noise.grain	= 0.2f;
	pp.noise.color.r = 200;
	pp.noise.color.g = 128;
	pp.noise.color.b = 200;
	pp.noise.color.a = 50;
	pp.gray			= 0.7f;
	pp.blur			= 0.6f;
	
	return TRUE;
}

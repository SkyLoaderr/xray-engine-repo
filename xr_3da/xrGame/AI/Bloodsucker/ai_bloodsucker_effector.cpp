#include "stdafx.h"
#include "ai_bloodsucker_effector.h" 

CBloodsuckerEffector::CBloodsuckerEffector(float time) 
					 : CEffectorPP(EEffectorPPType(BLOODSUCKER_EFFECTOR_TYPE_ID), time)
{
	fLifeTime	= time;
	this->time	= time;

	max_past				= 0.3f;

	max_power.duality.h     = 0.0f;
	max_power.duality.v     = 0.0f;
	max_power.noise.grain	= 1.f;
	max_power.noise.color.r = 255;
	max_power.noise.color.g = 255;
	max_power.noise.color.b = 255;
	max_power.noise.color.a = 255;
	max_power.noise.intensity = 0.5f;
	max_power.gray			= 0.7f;
	max_power.blur			= 1.0f;
}

CBloodsuckerEffector::~CBloodsuckerEffector()
{

}

void CBloodsuckerEffector::Init ()
{
}

BOOL CBloodsuckerEffector::Process(SPPInfo& pp)
{
	fLifeTime -= Device.fTimeDelta;
	inherited::Process(pp);
	
	float time_past = time - fLifeTime;
	float d = 1.f;

	if (time_past < max_past)				d = time_past / max_past; 			// grow
	else if (time_past > (time - max_past)) d = (time - time_past) / max_past;  // fade
	else {																		// static
		pp = max_power;
		return TRUE;
	}
	
	pp.duality.h		= max_power.duality.h * d;
	pp.duality.v		= max_power.duality.v * d;
	pp.noise.grain		= max_power.noise.grain * d;
	pp.noise.color.r	= max_power.noise.color.r * d;
	pp.noise.color.g	= max_power.noise.color.g * d;
	pp.noise.color.b	= max_power.noise.color.b * d; 
	pp.noise.color.a	= max_power.noise.color.a * d; 
	pp.gray				= max_power.gray * d; 
	pp.blur				= max_power.blur * d; 
	pp.noise.intensity  = max_power.noise.intensity *d;

	return TRUE;
}

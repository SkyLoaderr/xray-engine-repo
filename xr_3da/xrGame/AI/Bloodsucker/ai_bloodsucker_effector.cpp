#include "stdafx.h"
#include "ai_bloodsucker_effector.h" 

CBloodsuckerEffector::CBloodsuckerEffector(float time) 
					 : CEffectorPP(EEffectorPPType(BLOODSUCKER_EFFECTOR_TYPE_ID), time)
{
	fLifeTime	= time;
	this->time	= time;

	max_past				= 0.3f;

	max_power.duality.h     = 0.1f;
	max_power.duality.v     = 0.1f;
	max_power.noise.grain	= 1.f;
	max_power.noise.intensity = 0.5f;
	max_power.noise.fps		= 30;
	max_power.color_base.r	= 255;
	max_power.color_base.g	= 255;
	max_power.color_base.b	= 255;
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
//#pragma todo("Dima to Jim : Remove next line, because you substract value 2 times, first by yourself, second - by calling inherited")
//	fLifeTime -= Device.fTimeDelta;
	inherited::Process(pp);
	
	float time_past = time - fLifeTime;
	float d = 1.f;

	if (time_past < max_past)				d = time_past / max_past; 			// grow
	else if (time_past > (time - max_past)) d = (time - time_past) / max_past;  // fade
	else {																		// static
		pp = max_power;
		return TRUE;
	}
	
	if (fis_zero(max_power.noise.grain*d,EPS_L)) return TRUE;

	pp.duality.h		= max_power.duality.h * d;
	pp.duality.v		= max_power.duality.v * d;
	pp.noise.grain		= max_power.noise.grain * d;
	pp.color_base.r		= max_power.color_base.r * d;
	pp.color_base.g		= max_power.color_base.g * d;
	pp.color_base.b		= max_power.color_base.b * d; 
	pp.gray				= max_power.gray * d; 
	pp.blur				= max_power.blur * d; 
	pp.noise.intensity  = max_power.noise.intensity *d;

	return TRUE;
}

#include "stdafx.h"
#include "pp_effector_distance.h"
#include "level.h"

////////////////////////////////////////////////////////////////////////////////////
// CPPEffectDistance
////////////////////////////////////////////////////////////////////////////////////
#define PP_EFFECTOR_DISTANCE_TYPE_ID	4

CPPEffectDistance::CPPEffectDistance(const SPPInfo &ppi) :
	inherited(EEffectorPPType(PP_EFFECTOR_DISTANCE_TYPE_ID), flt_max)
{
	state = ppi;
	factor = 0.1f;
}

CPPEffectDistance::~CPPEffectDistance()
{
}

BOOL CPPEffectDistance::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	SPPInfo	def;

	pp.duality.h		= def.duality.h			+ (state.duality.h			- def.duality.h)		* factor; 			
	pp.duality.v		= def.duality.v			+ (state.duality.v			- def.duality.v)		* factor;
	pp.gray				= def.gray				+ (state.gray				- def.gray)				* factor;
	pp.blur				= def.blur				+ (state.blur				- def.blur)				* factor;
	pp.noise.intensity	= def.noise.intensity	+ (state.noise.intensity	- def.noise.intensity)	* factor;
	pp.noise.grain		= def.noise.grain		+ (state.noise.grain		- def.noise.grain)		* factor;
	pp.noise.fps		= def.noise.fps			+ (state.noise.fps			- def.noise.fps)		* factor;	
	VERIFY(!fis_zero(pp.noise.fps));

	pp.color_base.set	(
		def.color_base.r	+ (state.color_base.r - def.color_base.r) * factor, 
		def.color_base.g	+ (state.color_base.g - def.color_base.g) * factor, 
		def.color_base.b	+ (state.color_base.b - def.color_base.b) * factor
		);

	pp.color_gray.set	(
		def.color_gray.r	+ (state.color_gray.r - def.color_gray.r) * factor, 
		def.color_gray.g	+ (state.color_gray.g - def.color_gray.g) * factor, 
		def.color_gray.b	+ (state.color_gray.b - def.color_gray.b) * factor
		);

	pp.color_add.set	(
		def.color_add.r	+ (state.color_add.r - def.color_add.r) * factor, 
		def.color_add.g	+ (state.color_add.g - def.color_add.g) * factor, 
		def.color_add.b	+ (state.color_add.b - def.color_add.b) * factor
		);

	return TRUE;
}

void CPPEffectDistance::Destroy()
{
	fLifeTime = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////
// CPPEffectorDistance
////////////////////////////////////////////////////////////////////////////////////
CPPEffectorDistance::CPPEffectorDistance() 
{
	p_effector  = 0;
	radius		= 1;
}

CPPEffectorDistance::~CPPEffectorDistance()
{

}


void CPPEffectorDistance::Load(LPCSTR section)
{
	state.duality.h			= pSettings->r_float(section,"duality_h");
	state.duality.v			= pSettings->r_float(section,"duality_v");
	state.gray				= pSettings->r_float(section,"gray");
	state.blur				= pSettings->r_float(section,"blur");
	state.noise.intensity	= pSettings->r_float(section,"noise_intensity");
	state.noise.grain		= pSettings->r_float(section,"noise_grain");
	state.noise.fps			= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(state.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &state.color_base.r, &state.color_base.g, &state.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &state.color_gray.r, &state.color_gray.g, &state.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &state.color_add.r,	&state.color_add.g,	 &state.color_add.b);

	r_min_perc	= pSettings->r_float(section,"radius_min");
	r_max_perc	= pSettings->r_float(section,"radius_max");

	VERIFY(r_min_perc <= r_max_perc);
}

void CPPEffectorDistance::Activate()
{
	p_effector = xr_new<CPPEffectDistance>(state);
	Level().Cameras.AddEffector(p_effector);
}

void CPPEffectorDistance::Update(float dist)
{
	// count r_min && r_max
	float min_r = radius * r_min_perc;
	float max_r = radius * r_max_perc;

	if (p_effector) {
		if (dist > max_r) {
			p_effector->Destroy();
			p_effector = 0;
		}
	} else if (dist < max_r) Activate();

	if (p_effector) {
		float f = (max_r - dist) / (max_r - min_r);
		clamp(f,0.01f,1.0f);
		p_effector->Update(f);
	}
}

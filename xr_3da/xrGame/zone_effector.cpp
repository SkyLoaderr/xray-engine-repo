#include "stdafx.h"
#include "zone_effector.h"
#include "level.h"

////////////////////////////////////////////////////////////////////////////////////
// CZoneEffectPP
////////////////////////////////////////////////////////////////////////////////////

#define ZONE_EFFECTOR_TYPE_ID	4

CZoneEffectPP::CZoneEffectPP(const SPPInfo &ppi) :
	CEffectorPP(EEffectorPPType(ZONE_EFFECTOR_TYPE_ID), flt_max)
{
	state = ppi;
	factor = 0.1f;
}

CZoneEffectPP::~CZoneEffectPP()
{

}

BOOL CZoneEffectPP::Process(SPPInfo& pp)
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

void CZoneEffectPP::Destroy()
{
	fLifeTime = 0.f;
}


////////////////////////////////////////////////////////////////////////////////////
// CZoneEffector
////////////////////////////////////////////////////////////////////////////////////
CZoneEffector::CZoneEffector() 
{
	p_effector  = 0;
	radius		= 1;
}

CZoneEffector::~CZoneEffector()
{

}


void CZoneEffector::Load(LPCSTR section)
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

void CZoneEffector::Activate()
{
	p_effector = xr_new<CZoneEffectPP>(state);
	Level().Cameras.AddEffector(p_effector);
}

void CZoneEffector::Stop()
{
	if (!p_effector) return;
	
	p_effector->Destroy();
	p_effector = 0;
};

void CZoneEffector::Update(float dist)
{
	// count r_min && r_max
	float min_r = radius * r_min_perc;
	float max_r = radius * r_max_perc;

	bool CreateEffector = (Level().CurrentEntity() && Level().CurrentEntity()->SUB_CLS_ID == CLSID_OBJECT_ACTOR);
	
	if (p_effector) 
	{
		if (dist > max_r || !CreateEffector)
		{
			p_effector->Destroy();
			p_effector = 0;
		}
	} 
	else 
		if (dist < max_r && CreateEffector) Activate();
	
	if (p_effector) {
		float f = (max_r - dist) / (max_r - min_r);
		clamp(f,0.01f,1.0f);
		p_effector->Update(f);
	}
}

void CZoneEffector::SetParam(u32 type, float val)
{
	switch (PP_Type(type)) {
		case DUALITY_V:			state.duality.v			= val; break;
		case DUALITY_H:			state.duality.h			= val; break;
		case NOISE_INTENSITY:	state.noise.intensity	= val; break;
		case NOISE_GRAIN:		state.noise.grain		= val; break;
		case NOISE_FPS:			state.noise.fps			= val; VERIFY(!fis_zero(state.noise.fps));break;
		case BLUR:				state.blur				= val; break;
		case GRAY:				state.gray				= val; break;	
		case COLOR_BASE_R:		state.color_base.r		= val; break;
		case COLOR_BASE_G:		state.color_base.g		= val; break;
		case COLOR_BASE_B:		state.color_base.b		= val; break;
		case COLOR_GRAY_R:		state.color_gray.r		= val; break;
		case COLOR_GRAY_G:		state.color_gray.g		= val; break;
		case COLOR_GRAY_B:		state.color_gray.b		= val; break;
		case COLOR_ADD_R:		state.color_add.r		= val; break;
		case COLOR_ADD_G:		state.color_add.g		= val; break;
		case COLOR_ADD_B:		state.color_add.b		= val; break;
		case DIST_MIN:			r_min_perc				= val; break;
		case DIST_MAX:			r_max_perc				= val; break;
		default: NODEFAULT;
	}
}

void CZoneEffector::SetRadius(float r)
{
	radius	= r;
}


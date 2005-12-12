#include "stdafx.h"
#include "zone_effector.h"
#include "level.h"
#include "clsid_game.h"
#include "../xr_object.h"
#include "../cameramanager.h"
#include "actor.h"

CZoneEffectPP::CZoneEffectPP(const SPPInfo &ppi, EEffectorPPType type) :
	CEffectorPP(type, flt_max, false)
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

	clamp(factor,0.01f,1.0f);
	pp.lerp	(pp_identity, state, factor);

	return TRUE;
}

void CZoneEffectPP::Destroy()
{
	fLifeTime		= 0.f;
	CZoneEffectPP	*self = this;
	xr_delete		(self);
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
	m_pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!m_pActor) return;
	p_effector = xr_new<CZoneEffectPP>(state, EEffectorPPType( u32(u64(this) & u32(-1)) ));
	m_pActor->Cameras().AddPPEffector(p_effector);

}

void CZoneEffector::Stop()
{
	if (!p_effector) return;
	 
	m_pActor->Cameras().RemovePPEffector(EEffectorPPType( u32(u64(this) & u32(-1)) ));
	p_effector->Destroy();
	p_effector = 0;
};

void CZoneEffector::Update(float dist)
{
	// count r_min && r_max
	float min_r = radius * r_min_perc;
	float max_r = radius * r_max_perc;

	bool camera_on_actor = (Level().CurrentEntity() && (Level().CurrentEntity()->CLS_ID == CLSID_OBJECT_ACTOR));
	
	if (p_effector) {
		if ((dist > max_r) || !camera_on_actor || (m_pActor&&!m_pActor->g_Alive()))	Stop();
	} else {
		if ((dist < max_r) && camera_on_actor)	Activate();
	}
	
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


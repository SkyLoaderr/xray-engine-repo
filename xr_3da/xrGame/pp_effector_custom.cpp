#include "stdafx.h"
#include "pp_effector_custom.h"
#include "level.h"

#define TRANSLATE_TYPE(val) EEffectorPPType(val ? u32(u64(typeid(this).name())) : u32(u64(this) & u32(-1)))

#pragma warning(push)
#pragma warning(disable:4355) // 'this' : used in base member initializer list

CPPEffectorCustom::CPPEffectorCustom(const SPPInfo &ppi, bool one_instance, bool destroy_from_engine) :
inherited(TRANSLATE_TYPE(one_instance), flt_max, destroy_from_engine)
{
	m_state		= ppi;
	m_factor	= 0.f;
	m_type		= TRANSLATE_TYPE(one_instance);
}
#pragma warning(pop)



#define SET_VALUE(def, target, factor) (def + (target-def) * factor)

BOOL CPPEffectorCustom::Process(SPPInfo& pp)
{
	if (!inherited::Process(pp)) return FALSE;

	// update factor
	if (!update()) return FALSE;


	// apply
	SPPInfo	def;

	pp.duality.h		= SET_VALUE(def.duality.h,			m_state.duality.h,			m_factor);
	pp.duality.v		= SET_VALUE(def.duality.v,			m_state.duality.v,			m_factor);
	pp.gray				= SET_VALUE(def.gray,				m_state.gray,				m_factor);
	pp.blur				= SET_VALUE(def.blur,				m_state.blur,				m_factor);
	pp.noise.intensity	= SET_VALUE(def.noise.intensity,	m_state.noise.intensity,	m_factor);
	pp.noise.grain		= SET_VALUE(def.noise.grain,		m_state.noise.grain,		m_factor);
	pp.noise.fps		= SET_VALUE(def.noise.fps,			m_state.noise.fps,			m_factor);
	VERIFY(!fis_zero(pp.noise.fps));


	pp.color_base.set	( 
		SET_VALUE(def.color_base.r,	m_state.color_base.r,	m_factor),
		SET_VALUE(def.color_base.g,	m_state.color_base.g,	m_factor),
		SET_VALUE(def.color_base.b,	m_state.color_base.b,	m_factor)
		);

	pp.color_gray.set	(
		SET_VALUE(def.color_gray.r,	m_state.color_gray.r,	m_factor),
		SET_VALUE(def.color_gray.g,	m_state.color_gray.g,	m_factor),
		SET_VALUE(def.color_gray.b,	m_state.color_gray.b,	m_factor)
		);

	pp.color_add.set	(
		SET_VALUE(def.color_add.r,	m_state.color_add.r,	m_factor),
		SET_VALUE(def.color_add.g,	m_state.color_add.g,	m_factor),
		SET_VALUE(def.color_add.b,	m_state.color_add.b,	m_factor)
		);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CPPEffectorControlled::CPPEffectorControlled(CPPEffectorController *controller, const SPPInfo &ppi, bool one_instance , bool destroy_from_engine ) : inherited (ppi,one_instance,destroy_from_engine)
{
	m_controller = controller;
}
BOOL CPPEffectorControlled::update()
{
	m_controller->update_factor();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CPPEffectorController::CPPEffectorController() 
{
	m_effector  = 0;
}

CPPEffectorController::~CPPEffectorController()
{
	if (m_effector) {
		Level().Cameras.RemoveEffector(m_effector->get_type());
		xr_delete(m_effector);
	}
}

void CPPEffectorController::load(LPCSTR section)
{
	m_state.duality.h			= pSettings->r_float(section,"duality_h");
	m_state.duality.v			= pSettings->r_float(section,"duality_v");
	m_state.gray				= pSettings->r_float(section,"gray");
	m_state.blur				= pSettings->r_float(section,"blur");
	m_state.noise.intensity		= pSettings->r_float(section,"noise_intensity");
	m_state.noise.grain			= pSettings->r_float(section,"noise_grain");
	m_state.noise.fps			= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(m_state.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &m_state.color_base.r, &m_state.color_base.g, &m_state.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &m_state.color_gray.r, &m_state.color_gray.g, &m_state.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &m_state.color_add.r,	&m_state.color_add.g,	 &m_state.color_add.b);
}

void CPPEffectorController::activate()
{	
	VERIFY							(!m_effector);
	
	m_effector = create_effector	();
	Level().Cameras.AddEffector		(m_effector);
}

void CPPEffectorController::deactivate()
{
	VERIFY				(m_effector);
	
	xr_delete			(m_effector);
	m_effector			= 0;
}

void CPPEffectorController::frame_update()
{
	if (m_effector) {
		if (check_completion())				deactivate();
	} else if (check_start_conditions())	activate();
}




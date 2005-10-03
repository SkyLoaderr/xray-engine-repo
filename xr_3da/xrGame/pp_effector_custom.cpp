#include "stdafx.h"
#include "pp_effector_custom.h"
#include "level.h"

CPPEffectCustom::CPPEffectCustom(const SPPInfo &ppi, EEffectorPPType type) :
	inherited(type, flt_max, false)
{
	m_state		= ppi;
	m_factor	= 0.1f;
}

CPPEffectCustom::~CPPEffectCustom()
{
}

BOOL CPPEffectCustom::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	SPPInfo	def;

	pp.duality.h		= def.duality.h			+ (m_state.duality.h			- def.duality.h)		* m_factor; 			
	pp.duality.v		= def.duality.v			+ (m_state.duality.v			- def.duality.v)		* m_factor;
	pp.gray				= def.gray				+ (m_state.gray				- def.gray)				* m_factor;
	pp.blur				= def.blur				+ (m_state.blur				- def.blur)				* m_factor;
	pp.noise.intensity	= def.noise.intensity	+ (m_state.noise.intensity	- def.noise.intensity)	* m_factor;
	pp.noise.grain		= def.noise.grain		+ (m_state.noise.grain		- def.noise.grain)		* m_factor;
	pp.noise.fps		= def.noise.fps			+ (m_state.noise.fps			- def.noise.fps)		* m_factor;	
	VERIFY(!fis_zero(pp.noise.fps));

	pp.color_base.set	(
		def.color_base.r	+ (m_state.color_base.r - def.color_base.r) * m_factor, 
		def.color_base.g	+ (m_state.color_base.g - def.color_base.g) * m_factor, 
		def.color_base.b	+ (m_state.color_base.b - def.color_base.b) * m_factor
		);

	pp.color_gray.set	(
		def.color_gray.r	+ (m_state.color_gray.r - def.color_gray.r) * m_factor, 
		def.color_gray.g	+ (m_state.color_gray.g - def.color_gray.g) * m_factor, 
		def.color_gray.b	+ (m_state.color_gray.b - def.color_gray.b) * m_factor
		);

	pp.color_add.set	(
		def.color_add.r	+ (m_state.color_add.r - def.color_add.r) * m_factor, 
		def.color_add.g	+ (m_state.color_add.g - def.color_add.g) * m_factor, 
		def.color_add.b	+ (m_state.color_add.b - def.color_add.b) * m_factor
		);

	return TRUE;
}

void CPPEffectCustom::destroy()
{
	CPPEffectCustom	*self = this;
	xr_delete		(self);
}

CPPEffectorCustom::CPPEffectorCustom() 
{
	m_effector  = 0;
	m_factor	= 0.f;
}

CPPEffectorCustom::~CPPEffectorCustom()
{
}

void CPPEffectorCustom::load(LPCSTR section)
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

void CPPEffectorCustom::activate()
{	
	VERIFY(!m_effector);

	m_effector = xr_new<CPPEffectCustom>(m_state, EEffectorPPType( u32(u64(this) & u32(-1)) ));
	Level().Cameras.AddEffector(m_effector);
}

void CPPEffectorCustom::deactivate()
{
	VERIFY(m_effector);
	
	Level().Cameras.RemoveEffector(EEffectorPPType( u32(u64(this) & u32(-1)) ));
	m_effector->destroy	();
	m_effector			= 0;
}

void CPPEffectorCustom::update()
{
	if (m_effector) {
		if (check_completion())				deactivate();
	} else if (check_start_conditions())	activate();

	if (m_effector) {
		update_factor		();
		m_effector->update	(m_factor);
	}
}


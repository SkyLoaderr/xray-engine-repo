#include "stdafx.h"
#include "controller_psy_aura.h"
#include "controller.h"
#include "../../../actor.h"
#include "../../../level.h"

CPPEffectorControllerAura::CPPEffectorControllerAura(const SPPInfo &ppi, u32 time_to_fade)
: inherited(ppi)
{
	m_time_to_fade			= time_to_fade;
	m_effector_state		= eStateFadeIn;
	m_time_state_started	= Device.dwTimeGlobal;

}

void CPPEffectorControllerAura::switch_off()
{
	m_effector_state		= eStateFadeOut;		
	m_time_state_started	= Device.dwTimeGlobal;
}


BOOL CPPEffectorControllerAura::update()
{
	// update factor
	if (m_effector_state == eStatePermanent) {
		m_factor = 1.f;
	} else {
		m_factor = float(Device.dwTimeGlobal - m_time_state_started) / float(m_time_to_fade);
		if (m_effector_state == eStateFadeOut) m_factor = 1 - m_factor;

		if (m_factor > 1) {
			m_effector_state	= eStatePermanent;
			m_factor			= 1.f;
		} else if (m_factor < 0) {
			return FALSE;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void CControllerAura::update_schedule()
{
	if (!m_object->g_Alive()) return;

	
	bool need_be_active = (Actor()->Position().distance_to(m_object->Position()) < m_object->aura_radius);
	

	if (active()) {
		if (!need_be_active) {
			m_effector->switch_off	();
			m_effector				= 0;
		} else {
			float time_delta = float(time() - m_time_last_update) / 1000.f;
			m_object->Hit_Psy(Actor(), time_delta * m_object->aura_damage);
			m_time_last_update = time();
		}
	} else {
		if (need_be_active) {
			// create effector
			m_effector = xr_new<CPPEffectorControllerAura>	(m_state, 5000);
			Actor()->Cameras().AddPPEffector				(m_effector);
			m_time_last_update								= time();
		}
	}

}

void CControllerAura::on_death()
{
	if (active()) {
		m_effector->switch_off	();
		m_effector				= 0;
	}
}


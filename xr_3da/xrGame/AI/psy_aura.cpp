#include "stdafx.h"
#include "psy_aura.h"
#include "../level.h"
#include "../gameobject.h"

CPsyAura::CPsyAura()
{
	object						= 0;
	m_radius					= 1.f;
	m_auto_activate				= false;
}

CPsyAura::~CPsyAura()
{
}

void CPsyAura::reinit()
{
	m_active					= false;
	m_power.value				= 1.0f;
	m_power.time_last_update	= Level().timeServer();
}

void CPsyAura::reload(LPCSTR section) 
{
	m_power.restore_vel		= pSettings->r_float(section, "PsyAura_Restore_Velocity");	
	m_power.decline_vel		= pSettings->r_float(section, "PsyAura_Decline_Velocity");	
	m_power.critical_value	= pSettings->r_float(section, "PsyAura_Critical_Value");
	m_power.activate_value	= pSettings->r_float(section, "PsyAura_Activate_Value");
}

void CPsyAura::activate()
{
	if (!is_active()) on_activate();		
	m_active = true;
}	

void CPsyAura::deactivate()
{
	if (is_active()) on_deactivate();	
	m_active = false;
}

void CPsyAura::schedule_update()
{
	update_power();
	
	if (is_active()){
		feel_touch_update(object->Position(), m_radius);
		process_objects_in_aura();
	}
}

void CPsyAura::update_power()
{	
	u32		cur_time	= Level().timeServer();
	float	dt			= float(cur_time - m_power.time_last_update) / 1000.f;
	
	if (!is_active()) 
		m_power.value += m_power.restore_vel * dt;
	else 
		m_power.value -= m_power.decline_vel * dt;

	clamp(m_power.value, 0.f, 1.f);
	m_power.time_last_update = cur_time;

	// проверка на включение/выключение поля
	if (m_power.value < m_power.critical_value) deactivate();
	if (m_auto_activate && can_activate()) activate();
}


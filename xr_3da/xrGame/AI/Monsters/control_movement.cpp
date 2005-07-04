#include "stdafx.h"
#include "control_movement.h"
#include "BaseMonster/base_monster.h"
#include "control_manager.h"

void CControlMovement::reinit()
{
	inherited::reinit	();

	m_velocity_current	= 0.f;
}

void CControlMovement::reset_data() 
{
	m_data.velocity_target	= 0.f; 
	m_data.acc				= flt_max;
}

void CControlMovement::update_frame()
{
	velocity_lerp								(m_velocity_current, m_data.velocity_target, m_data.acc, Device.fTimeDelta);

	m_object->m_fCurSpeed						=	m_velocity_current;
	m_man->path_builder().set_desirable_speed	(m_velocity_current);
}


#include "stdafx.h"
#include "control_movement.h"
#include "BaseMonster/base_monster.h"
#include "control_manager.h"
#include "../../PHMovementControl.h"

#define PREVIOUS	0
#define CURRENT		1

void CControlMovement::reinit()
{
	inherited::reinit		();

	m_velocity_current		= 0.f;

	m_data.velocity_target	= 0.f; 
	m_data.acc				= flt_max;

	m_pos_stack[PREVIOUS].position	= m_object->Position();
	m_pos_stack[PREVIOUS].time		= 0;

	m_pos_stack[CURRENT].position	= m_object->Position();
	m_pos_stack[CURRENT].time		= 1;
}

void CControlMovement::update_frame()
{
	velocity_lerp								(m_velocity_current, m_data.velocity_target, m_data.acc, Device.fTimeDelta);

	m_object->m_fCurSpeed						=	m_velocity_current;
	m_man->path_builder().set_desirable_speed	(m_velocity_current);

	// update position stack
	if (m_pos_stack[CURRENT].position.similar(m_object->Position())) {
		m_pos_stack[CURRENT].time		= Device.dwTimeGlobal;
	} else {
		m_pos_stack[PREVIOUS].position	= m_pos_stack[CURRENT].position;
		m_pos_stack[PREVIOUS].time		= m_pos_stack[CURRENT].time;

		m_pos_stack[CURRENT].position	= m_object->Position();
		m_pos_stack[CURRENT].time		= Device.dwTimeGlobal;
	}
}

float CControlMovement::real_velocity()
{
	CPHMovementControl		*movement_control = m_object->movement_control();
	VERIFY					(movement_control);

	if (movement_control->IsCharacterEnabled())
		return				(movement_control->GetVelocityActual());

	//return	(m_pos_stack[CURRENT].position.distance_to(m_pos_stack[PREVIOUS].position)/ (float(m_pos_stack[CURRENT].time - m_pos_stack[PREVIOUS].time)/1000.f));
	return m_velocity_current;
}
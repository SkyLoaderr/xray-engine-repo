////////////////////////////////////////////////////////////////////////////
//	Module 		: setup_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker setup action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSetupAction::CSetupAction				(float weight, u32 inertia_time) :
	m_weight						(weight),
	m_inertia_time					(inertia_time)
{
}

IC	void CSetupAction::set_object			(CAI_Stalker *object)
{
	VERIFY							(object);
	m_object						= object;
	m_animation_action.set_object	(object);
	m_movement_action.set_object	(object);
	m_object_action.set_object		(object);
	m_sight_action.set_object		(object);
	m_sound_action.set_object		(object);
}

IC	CSetupAction &CSetupAction::animation	(const CAnimationAction &animation_action)
{
	m_animation_action				= animation_action;
}

IC	CSetupAction &CSetupAction::movement	(const CMovementAction &movement_action)
{
	m_movement_action				= movement_action;
}

IC	CSetupAction &CSetupAction::object		(const CObjectAction &object_action)
{
	m_object_action					= object_action;
}

IC	CSetupAction &CSetupAction::sight		(const CSightAction	&sight_action)
{
	m_sight_action					= sight_action;
}

IC	CSetupAction &CSetupAction::sound		(const CSoundAction	&sound_action)
{
	m_sound_action					= sound_action;
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	BOOL CAI_Stalker::UsedAI_Locations				()
{
	return	(TRUE);
}

IC	CStalkerAnimationManager &CAI_Stalker::animation() const
{
	VERIFY	(m_animation_manager);
	return	(*m_animation_manager);
}

IC	CMotivationActionManagerStalker &CAI_Stalker::brain() const
{
	VERIFY	(m_brain);
	return	(*m_brain);
}

IC	CSightManager &CAI_Stalker::sight				() const
{
	VERIFY	(m_sight_manager);
	return	(*m_sight_manager);
}

IC	CAI_Stalker::CSSetupManager &CAI_Stalker::setup	() const
{
	VERIFY	(m_setup_manager);
	return	(*m_setup_manager);
}

IC	LPCSTR CAI_Stalker::Name						() const
{
	return	(CInventoryOwner::Name());
}

IC	float CAI_Stalker::panic_threshold				() const
{
	return	(m_panic_threshold);
}

IC	void CAI_Stalker::body_action					(const EBodyAction &body_action)
{
	m_body_action	= body_action;
}

IC	const StalkerSpace::EBodyAction	&CAI_Stalker::body_action() const
{
	return			(m_body_action);
}

IC	CStalkerMovementManager	&CAI_Stalker::movement	() const
{
	VERIFY			(m_movement_manager);
	return			(*m_movement_manager);
}

IC	bool CAI_Stalker::frame_check					(u32 &frame)
{
	if (Device.dwFrame == frame)
		return		(false);

	frame			= Device.dwFrame;
	return			(true);
}

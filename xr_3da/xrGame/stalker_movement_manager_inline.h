////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC const CMovementManager::SBoneRotation &CStalkerMovementManager::head_orientation() const
{
	return				(m_head);
}

IC const MonsterSpace::EMovementType CStalkerMovementManager::movement_type() const
{
	return				(m_tMovementType);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::mental_state() const
{
	return				(m_tMentalState);
}

IC void CStalkerMovementManager::set_head_orientation(const CMovementManager::SBoneRotation &orientation)
{
	m_head				= orientation;
}

IC	const MonsterSpace::EBodyState CStalkerMovementManager::body_state() const
{
	return				(m_tBodyState);
}

IC	void CStalkerMovementManager::set_mental_state	(const MonsterSpace::EMentalState mental_state)
{
	m_tMentalState		= mental_state;
}


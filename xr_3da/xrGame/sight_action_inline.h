////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_action_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 03.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sight action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CSightAction::use_torso_look	() const
{
	return				(m_torso_look);
}

IC	float CSightAction::weight			() const
{
	return				(m_weight);
}

IC	CSightAction::ESightType CSightAction::sight_type	() const
{
	return				(m_sight_type);
}

IC	const Fvector &CSightAction::vector3d	() const
{
	return				(m_vector3d);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain_inline.h
//	Created 	: 06.10.2005
//  Modified 	: 06.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeHumanBrain::object_type &CALifeHumanBrain::object				() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

IC	CALifeHumanBrain::object_handler_type &CALifeHumanBrain::objects	() const
{
	VERIFY		(m_object_handler);
	return		(*m_object_handler);
}

IC	CALifeHumanBrain::movement_manager_type &CALifeHumanBrain::movement	() const
{
	VERIFY		(m_movement_manager);
	return		(*m_movement_manager);
}

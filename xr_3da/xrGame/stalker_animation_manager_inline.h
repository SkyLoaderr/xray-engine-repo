////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_manager_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CStalkerAnimationManager::SCRIPT_ANIMATIONS &CStalkerAnimationManager::script_animations	() const
{
	return	(m_script_animations);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::global	()
{
	return	(m_global);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::head	()
{
	return	(m_head);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::torso	()
{
	return	(m_torso);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::legs	()
{
	return	(m_legs);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::script	()
{
	return	(m_script);
}

IC	CAI_Stalker	*CStalkerAnimationManager::object			()
{
	VERIFY	(m_object);
	return	(m_object);
}

IC	const CAI_Stalker *CStalkerAnimationManager::object		() const
{
	VERIFY	(m_object);
	return	(m_object);
}

IC	void CStalkerAnimationManager::pop_script_animation		()
{
	VERIFY	(!script_animations().empty());
	m_script_animations.pop_front();
	script().reset();
}

IC	void CStalkerAnimationManager::clear_script_animations	()
{
	m_script_animations.clear		();
	script().reset();
}

IC	void CStalkerAnimationManager::setup_storage			(CPropertyStorage *storage)
{
	VERIFY			(!m_storage || (m_storage && !storage));
	if (!storage)
		Msg			("%6d : STORAGE IS CLEARED",Device.dwTimeGlobal);
	else
		Msg			("%6d : STORAGE IS SETUP",Device.dwTimeGlobal);
	m_storage		= storage;
}

IC	CPropertyStorage *CStalkerAnimationManager::setup_storage	() const
{
	return			(m_storage);
}

IC	CStalkerAnimationManager::_condition_type CStalkerAnimationManager::property_id	() const
{
	return			(m_property_id);
}

IC	void CStalkerAnimationManager::property_id				(_condition_type value)
{
	m_property_id	= value;
}

IC	CStalkerAnimationManager::_value_type CStalkerAnimationManager::property_value	() const
{
	return			(m_property_value);
}

IC	void CStalkerAnimationManager::property_value			(_value_type value)
{
	m_property_value= value;
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_script_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class with script support (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		template <typename _object_type> class _motivation_type\
	>

#define CSMotivationActionManager CMotivationActionManagerScript<_object_type,_motivation_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationActionManager::CMotivationActionManagerScript	()
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
CSMotivationActionManager::~CMotivationActionManagerScript	()
{
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::reinit	(_object_type *object, bool clear_all)
{
	VERIFY				(object);
	inherited::reinit	(object->lua_game_object(),clear_all);
	m_object			= object;
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationActionManager
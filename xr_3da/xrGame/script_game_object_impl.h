////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_impl.h
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "ai_space.h"
#include "script_engine.h"

IC	CGameObject *CScriptGameObject::object	() const
{
	if (m_game_object && m_game_object->lua_game_object() == this)
		return	(m_game_object);

#ifdef DEBUG
	ai().script_engine().script_stack_tracker().print_stack(
		ai().script_engine().current_thread() ?
		ai().script_engine().current_thread()->lua() :
		ai().script_engine().lua()
	);
	VERIFY2	(m_game_object && m_game_object->lua_game_object() == this,"Probably, you are trying to use a destroyed object!");
#endif
	return	(m_game_object);
}


////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CPropertyEvaluatorWrapper::CPropertyEvaluatorWrapper	(luabind::weak_ref lua_instance, CLuaGameObject *object) : 
	CScriptPropertyEvaluator	(object),
	m_lua_instance				(lua_instance)
{
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property_evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CScriptPropertyEvaluator : public CPropertyEvaluator<CLuaGameObject> {
protected:
	typedef CPropertyEvaluator<CLuaGameObject> inherited;
public:
	IC					CScriptPropertyEvaluator	(CLuaGameObject *object = 0)
	{
		m_object			= object;
	}

	virtual void		reinit				(CLuaGameObject *object)
	{
		inherited::reinit	(object);
	}

	virtual	_value_type	evaluate			()
	{
		return				(inherited::evaluate());
	}
};

class CPropertyEvaluatorWrapper : public CScriptPropertyEvaluator {
public:
	luabind::object		m_lua_instance;

					CPropertyEvaluatorWrapper	(const luabind::object &lua_instance, CLuaGameObject *object = 0) : 
						CScriptPropertyEvaluator(object),
						m_lua_instance			(lua_instance)
	{
	}

	virtual void	reinit				(CLuaGameObject *object)
	{
		luabind::call_member<void>(m_lua_instance,"reinit",object);
	}

	static	void	reinit_static		(CScriptPropertyEvaluator *evaluator, CLuaGameObject *object)
	{
		evaluator->CScriptPropertyEvaluator::reinit(object);
	}

	virtual bool	evaluate			()
	{
		return		(luabind::call_member<bool>(m_lua_instance,"evaluate"));
	}

	static	bool	evaluate_static		(CScriptPropertyEvaluator *evaluator)
	{
		return		(evaluator->CScriptPropertyEvaluator::evaluate());
	}
};
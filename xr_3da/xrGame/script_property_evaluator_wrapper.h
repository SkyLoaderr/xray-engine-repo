////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"
#include "ai_script_space.h"

typedef CPropertyEvaluator<CLuaGameObject> CScriptPropertyEvaluator;

class CPropertyEvaluatorWrapper : public CScriptPropertyEvaluator {
public:
	luabind::object		m_lua_instance;

	IC					CPropertyEvaluatorWrapper	(const luabind::object &lua_instance, CLuaGameObject *object = 0);
	virtual void		reinit						(CLuaGameObject *object, CPropertyStorage *storage);
	static	void		reinit_static				(CScriptPropertyEvaluator *evaluator, CLuaGameObject *object, CPropertyStorage *storage);
	virtual bool		evaluate					();
	static	bool		evaluate_static				(CScriptPropertyEvaluator *evaluator);
};

#include "script_property_evaluator_wrapper_inline.h"
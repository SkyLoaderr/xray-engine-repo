////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_property_evaluator_wrapper.h"

void CPropertyEvaluatorWrapper::reinit			(CLuaGameObject *object, CPropertyStorage *storage)
{
	luabind::call_member<void>(m_lua_instance,"reinit",object,storage);
}

void CPropertyEvaluatorWrapper::reinit_static	(CScriptPropertyEvaluator *evaluator, CLuaGameObject *object, CPropertyStorage *storage)
{
	evaluator->CScriptPropertyEvaluator::reinit(object,storage);
}

bool CPropertyEvaluatorWrapper::evaluate		()
{
	return		(luabind::call_member<bool>(m_lua_instance,"evaluate"));
}

bool CPropertyEvaluatorWrapper::evaluate_static	(CScriptPropertyEvaluator *evaluator)
{
	return		(evaluator->CScriptPropertyEvaluator::evaluate());
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_property_evaluator_wrapper.h"
#include "ai_script_classes.h"

void CPropertyEvaluatorWrapper::reinit			(CLuaGameObject *object, CPropertyStorage *storage)
{
	call_member<void>("reinit",object,storage);
}

void CPropertyEvaluatorWrapper::reinit_static	(CScriptPropertyEvaluator *evaluator, CLuaGameObject *object, CPropertyStorage *storage)
{
	evaluator->CScriptPropertyEvaluator::reinit(object,storage);
}

bool CPropertyEvaluatorWrapper::evaluate		()
{
	return		(call_member<bool>("evaluate"));
}

bool CPropertyEvaluatorWrapper::evaluate_static	(CScriptPropertyEvaluator *evaluator)
{
	return		(evaluator->CScriptPropertyEvaluator::evaluate());
}

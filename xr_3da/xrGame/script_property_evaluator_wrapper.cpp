////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_property_evaluator_wrapper.h"
#include "script_game_object.h"

void CScriptPropertyEvaluatorWrapper::reinit			(CScriptGameObject *object, CPropertyStorage *storage)
{
	call_member<void>("reinit",object,storage);
}

void CScriptPropertyEvaluatorWrapper::reinit_static	(CScriptPropertyEvaluator *evaluator, CScriptGameObject *object, CPropertyStorage *storage)
{
	evaluator->CScriptPropertyEvaluator::reinit(object,storage);
}

bool CScriptPropertyEvaluatorWrapper::evaluate		()
{
	return		(call_member<bool>("evaluate"));
}

bool CScriptPropertyEvaluatorWrapper::evaluate_static	(CScriptPropertyEvaluator *evaluator)
{
	return		(evaluator->CScriptPropertyEvaluator::evaluate());
}

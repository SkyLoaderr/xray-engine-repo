////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"
#include "script_space.h"

typedef CPropertyEvaluator<CLuaGameObject> CScriptPropertyEvaluator;

class CPropertyEvaluatorWrapper : public CScriptPropertyEvaluator, public luabind::wrap_base {
public:
	IC					CPropertyEvaluatorWrapper	(CLuaGameObject *object = 0);
	virtual void		reinit						(CLuaGameObject *object, CPropertyStorage *storage);
	static	void		reinit_static				(CScriptPropertyEvaluator *evaluator, CLuaGameObject *object, CPropertyStorage *storage);
	virtual bool		evaluate					();
	static	bool		evaluate_static				(CScriptPropertyEvaluator *evaluator);
};

#include "script_property_evaluator_wrapper_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_properties.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager properties
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CAgentManager;

typedef CPropertyEvaluator<CAgentManager>		CAgentManagerPropertyEvaluator;
typedef CPropertyEvaluatorConst<CAgentManager>	CAgentManagerPropertyEvaluatorConst;
typedef CPropertyEvaluatorMember<CAgentManager>	CAgentManagerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorGlobal
//////////////////////////////////////////////////////////////////////////

class CAgentManagerPropertyEvaluatorGlobal : public CAgentManagerPropertyEvaluator {
protected:
	typedef CAgentManagerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

#include "agent_manager_properties_inline.h"
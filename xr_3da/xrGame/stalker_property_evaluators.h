////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wrapper_abstract.h"
#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CAI_Stalker;

typedef CWrapperAbstract2<CAI_Stalker,CPropertyEvaluator>		CStalkerPropertyEvaluator;
typedef CWrapperAbstract2<CAI_Stalker,CPropertyEvaluatorConst>	CStalkerPropertyEvaluatorConst;
typedef CWrapperAbstract2<CAI_Stalker,CPropertyEvaluatorMember>	CStalkerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorALife : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAlive : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

#include "stalker_property_evaluators_inline.h"
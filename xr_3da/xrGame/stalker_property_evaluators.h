////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CAI_Stalker;

typedef CPropertyEvaluatorConst<CAI_Stalker>	CStalkerPropertyEvaluatorConst;
typedef CPropertyEvaluatorMember<CAI_Stalker>	CStalkerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorALife : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAlive : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
	virtual _value_type	evaluate						();
};

#include "stalker_property_evaluators_inline.h"
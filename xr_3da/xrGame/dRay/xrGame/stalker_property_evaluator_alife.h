////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluator_alife.h
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CAI_Stalker;

class CStalkerPropertyEvaluatorALife : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
	virtual _value_type	evaluate						();
};
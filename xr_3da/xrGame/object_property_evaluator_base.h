////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_base.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CAI_Stalker;

class CObjectPropertyEvaluatorBase : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
};

#include "object_property_evaluator_base_inline.h"
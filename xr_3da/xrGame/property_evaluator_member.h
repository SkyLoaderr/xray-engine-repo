////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_member.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator member
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

template <typename _object_type>
class CPropertyEvaluatorMember : public CPropertyEvaluator<_object_type> {
protected:
	typedef CPropertyEvaluator<_object_type>		inherited;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::CSolverConditionStorage	CConditionStorage;

protected:
	const CConditionStorage		*m_storage;
	_condition_type				m_condition_id;
	_value_type					m_value;
	bool						m_equality;

public:
						CPropertyEvaluatorMember(const CConditionStorage *storage, _condition_type condition_id, _value_type value, bool equality = true);
	virtual _value_type	evaluate				();
};


#include "property_evaluator_member_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_member_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator member inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type\
	>
#define CEvaluator	CPropertyEvaluatorMember<_object_type>

TEMPLATE_SPECIALIZATION
CEvaluator::CPropertyEvaluatorMember	(const CConditionStorage *storage, _condition_type condition_id, _value_type value, bool equality) :
	m_storage			(storage),
	m_condition_id		(condition_id),
	m_value				(value),
	m_equality			(equality)
{
}

TEMPLATE_SPECIALIZATION
typename CEvaluator::_value_type CEvaluator::evaluate	()
{
	VERIFY								(m_storage);
	CConditionStorage::const_iterator	I = m_storage->find(m_condition_id);
	VERIFY								(m_storage->end() != I);
	return								(_value_type(((*I).second == m_value) == m_equality));
}

#undef TEMPLATE_SPECIALIZATION
#undef CEvaluator
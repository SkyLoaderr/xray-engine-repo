////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_member_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator member inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _object_type>
CPropertyEvaluatorMember<_object_type>::CPropertyEvaluatorMember	(const CConditionStorage *storage, _condition_type condition_id, _value_type value, bool equality) :
	m_storage			(storage),
	m_condition_id		(condition_id),
	m_value				(value),
	m_equality			(equality)
{
}

template <typename _object_type>
typename CPropertyEvaluatorMember<_object_type>::_value_type	CPropertyEvaluatorMember<_object_type>::evaluate	()
{
	VERIFY								(m_storage);
	CConditionStorage::const_iterator	I = m_storage->find(m_condition_id);
	VERIFY								(m_storage->end() != I);
	return								(_value_type(((*I).second == m_value) == m_equality));
}
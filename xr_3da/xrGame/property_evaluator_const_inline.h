////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_const_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator const inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _object_type>
CPropertyEvaluatorConst<_object_type>::CPropertyEvaluatorConst	(_value_type value) :
	m_value			(value)
{
}

template <typename _object_type>
typename CPropertyEvaluatorConst<_object_type>::_value_type	CPropertyEvaluatorConst<_object_type>::evaluate	()
{
	return			(m_value);
}

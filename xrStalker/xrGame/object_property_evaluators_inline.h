////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluators_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object property evaluators inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _item_type>
IC	CObjectPropertyEvaluatorBase<_item_type>::CObjectPropertyEvaluatorBase(_item_type *item, CAI_Stalker *owner)
{
	m_object		= owner;
	m_item			= item;
}
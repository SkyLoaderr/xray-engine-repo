////////////////////////////////////////////////////////////////////////////
//	Module 		: wrapper_abstract_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Abastract wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		template <typename _base_object_type> class ancestor\
	>
#define CWrapper CWrapperAbstract<_object_type,ancestor<_base_object_type> >

TEMPLATE_SPECIALIZATION
CWrapper::~CWrapperAbstract		()
{
}

TEMPLATE_SPECIALIZATION
void CWrapper::reinit			(_object_type *object)
{
	_base_object_type		*ancestor_object = dynamic_cast<_base_object_type>(object);
	VERIFY					(ancestor_object);
	inherited::reinit		(ancestor_object);
	m_object				= object;
}

#undef TEMPLATE_SPECIALIZATION
#undef CWrapper
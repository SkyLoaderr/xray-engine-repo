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
		template <typename _base_object_type> class ancestor,\
		bool reinit2,\
		typename _base_object_type\
	>
#define CWrapper CWrapperAbstract<_object_type,ancestor,reinit2,_base_object_type>

TEMPLATE_SPECIALIZATION
IC	CWrapper::CWrapperAbstract	()
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
template <typename T1>
IC	CWrapper::CWrapperAbstract	(T1 t1) :
	inherited			(t1)
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
template <typename T1, typename T2, typename T3>
IC	CWrapper::CWrapperAbstract	(T1 t1, T2 t2, T3 t3) :
	inherited			(t1,t2,t3)
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
CWrapper::~CWrapperAbstract		()
{
}

TEMPLATE_SPECIALIZATION
void CWrapper::reinit				(_object_type *object)
{
	VERIFY					(object);
	inherited::reinit		(object->lua_game_object());
	m_object				= object;
}

#undef TEMPLATE_SPECIALIZATION
#undef CWrapper
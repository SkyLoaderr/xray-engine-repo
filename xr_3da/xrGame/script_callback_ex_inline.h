////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION			template <typename _return_type>
#define CSScriptCallbackEx				CScriptCallbackEx_<_return_type>

TEMPLATE_SPECIALIZATION
IC	CSScriptCallbackEx::CScriptCallbackEx_	() 
{
	init				();
}

TEMPLATE_SPECIALIZATION
IC	CSScriptCallbackEx::~CScriptCallbackEx_	()
{
	clear				();
}

TEMPLATE_SPECIALIZATION
IC	void CSScriptCallbackEx::init			()
{
	m_functor			= 0;
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
IC	void CSScriptCallbackEx::clear			()
{
	try {
		xr_delete		(m_functor);
		xr_delete		(m_object);
	}
	catch(...) {
	}

#ifdef DEBUG
	init				();
#endif
}

TEMPLATE_SPECIALIZATION
IC	CSScriptCallbackEx::CScriptCallbackEx_	(const CScriptCallbackEx_ &callback)
{
	init				();

	if (callback.m_functor)
		m_functor		= xr_new<functor_type>(*callback.m_functor);

	if (callback.m_object)
		m_object		= xr_new<object_type>(*callback.m_object);
}

TEMPLATE_SPECIALIZATION
IC	void CSScriptCallbackEx::set			(const functor_type &functor)
{
	clear				();
	m_functor			= xr_new<functor_type>	(functor);
}

TEMPLATE_SPECIALIZATION
IC	void CSScriptCallbackEx::set			(const functor_type &functor, const object_type &object)
{
	clear				();
	m_functor			= xr_new<functor_type>	(functor);
	m_object			= xr_new<object_type>	(object);
}

TEMPLATE_SPECIALIZATION
IC	typename CSScriptCallbackEx::functor_type *CSScriptCallbackEx::functor	() const
{
	return				(m_functor);
}

TEMPLATE_SPECIALIZATION
IC	typename CSScriptCallbackEx::object_type	*CSScriptCallbackEx::object	() const
{
	return				(m_object);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSScriptCallbackEx
////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_ptr_inline.h
//	Created 	: 30.07.2004
//  Modified 	: 30.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Intrusive pointer template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename object_type, typename base_type>
#define CSIntrusivePtr CIntrusivePtr<object_type,base_type>

TEMPLATE_SPECIALIZATION
IC	CSIntrusivePtr::CIntrusivePtr	()
{
	m_object		= 0;
}

TEMPLATE_SPECIALIZATION
IC	CSIntrusivePtr::CIntrusivePtr	(object_type* rhs)
{
	m_object		= 0;
	set				(rhs);
}

TEMPLATE_SPECIALIZATION
IC	CSIntrusivePtr::CIntrusivePtr	(self_type const &rhs)
{
	m_object		= 0;
	set				(rhs);
}

TEMPLATE_SPECIALIZATION
IC	CSIntrusivePtr::~CIntrusivePtr	()
{
	STATIC_CHECK	(result,Class_MUST_Be_Derived_From_The_Base);
	dec				();
}

TEMPLATE_SPECIALIZATION
IC	void CSIntrusivePtr::dec		()
{
	if (!m_object)
		return;
	
	--m_object->base_type::m_ref_count;
	if (!m_object->base_type::m_ref_count)
		m_object->base_type::_release	(m_object);
}

TEMPLATE_SPECIALIZATION
IC	typename CSIntrusivePtr::self_type &CSIntrusivePtr::operator=	(object_type* rhs)
{
	set				(rhs);
	return			((self_type&)*this);
}

TEMPLATE_SPECIALIZATION
IC	typename CSIntrusivePtr::self_type &CSIntrusivePtr::operator=	(self_type const &rhs)
{
	set				(rhs);
	return			((self_type&)*this);
}

TEMPLATE_SPECIALIZATION
IC	typename CSIntrusivePtr::object_type &CSIntrusivePtr::operator*	() const
{
	VERIFY			(m_object);
	return			(*m_object);
}

TEMPLATE_SPECIALIZATION
IC	typename CSIntrusivePtr::object_type *CSIntrusivePtr::operator->() const
{
	VERIFY			(m_object);
	return			(m_object);
}

TEMPLATE_SPECIALIZATION
IC	bool CSIntrusivePtr::operator!	() const
{
	return			(!m_object);
}

TEMPLATE_SPECIALIZATION
IC	void CSIntrusivePtr::swap		(self_type & rhs)
{
	object_type		*tmp = m_object; 
	m_object		= rhs.m_object; 
	rhs.m_object	= tmp;	
}

TEMPLATE_SPECIALIZATION
IC	bool CSIntrusivePtr::equal		(const self_type & rhs)	const
{
	return			(m_object == rhs.m_object);
}

TEMPLATE_SPECIALIZATION
IC	void CSIntrusivePtr::set		(object_type* rhs)
{
	if (m_object == rhs)
		return;
	dec							();
	m_object					= rhs;
	if (!m_object)
		return;
	++m_object->m_ref_count;
}

TEMPLATE_SPECIALIZATION
IC	void CSIntrusivePtr::set		(self_type const &rhs)
{
	if (m_object == rhs.m_object)
		return;
	dec				();
	m_object		= rhs.m_object;
	if (!m_object)
		return;
	++m_object->m_ref_count;
}

TEMPLATE_SPECIALIZATION
IC	const typename CSIntrusivePtr::object_type* CSIntrusivePtr::get	()	const
{
	return			(m_object);
}

TEMPLATE_SPECIALIZATION
IC bool operator==					(CSIntrusivePtr const &a, CSIntrusivePtr const &b)		
{
	return			(a.get() == b.get());
}

TEMPLATE_SPECIALIZATION
IC bool operator	!=				(CSIntrusivePtr const &a, CSIntrusivePtr const &b)
{
	return			(a.get() != b.get());
}

TEMPLATE_SPECIALIZATION
IC bool operator<					(CSIntrusivePtr const &a, CSIntrusivePtr const &b)
{
	return			(a.get() <  b.get());
}

TEMPLATE_SPECIALIZATION
IC bool operator>					(CSIntrusivePtr const &a, CSIntrusivePtr const &b)
{
	return			(a.get() >  b.get());
}

TEMPLATE_SPECIALIZATION
IC void swap						(CSIntrusivePtr &lhs, CSIntrusivePtr &rhs)
{
	lhs.swap		(rhs);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSIntrusivePtr
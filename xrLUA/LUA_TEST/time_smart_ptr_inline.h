////////////////////////////////////////////////////////////////////////////
//	Module 		: time_smart_ptr_inline.h
//	Created 	: 30.07.2004
//  Modified 	: 30.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart pointer with time template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename T>
#define CSTimeSmartPtr CTimeSmartPtr<T>

TEMPLATE_SPECIALIZATION
IC	CSTimeSmartPtr::CTimeSmartPtr	()
{
	m_object		= 0;
	m_ref_count		= 0;
	m_last_time_dec	= 0;
}

TEMPLATE_SPECIALIZATION
IC	CSTimeSmartPtr::CTimeSmartPtr	(object_type* rhs)
{
	m_object		= 0;
	set				(rhs);
}

TEMPLATE_SPECIALIZATION
IC	CSTimeSmartPtr::CTimeSmartPtr	(self_type const &rhs)
{
	m_object		= 0;
	m_ref_count		= 0;
	m_last_time_dec	= 0;
	set				(rhs);
}

TEMPLATE_SPECIALIZATION
IC	CSTimeSmartPtr::~CTimeSmartPtr	()
{
	dec				();
}

TEMPLATE_SPECIALIZATION
IC	void CSTimeSmartPtr::dec		()
{
	if (!m_object)
		return;
	
	--*m_ref_count;
	*m_last_time_dec = timeGetTime();
	if (!*m_ref_count) {
		xr_delete	(m_object);
		xr_delete	(m_ref_count);
		xr_delete	(m_last_time_dec);
	}
}

TEMPLATE_SPECIALIZATION
IC	typename CSTimeSmartPtr::self_type&	CSTimeSmartPtr::operator=	(object_type* rhs)
{
	set				(rhs);
	return			((self_type&)*this);
}

TEMPLATE_SPECIALIZATION
IC	typename CSTimeSmartPtr::self_type&	CSTimeSmartPtr::operator=	(self_type const &rhs)
{
	set				(rhs);
	return			((self_type&)*this);
}

TEMPLATE_SPECIALIZATION
IC	typename CSTimeSmartPtr::object_type& CSTimeSmartPtr::operator*	() const
{
	VERIFY			(m_object);
	return			(*m_object);
}

TEMPLATE_SPECIALIZATION
IC	bool CSTimeSmartPtr::operator!	() const
{
	return			(!m_object);
}

TEMPLATE_SPECIALIZATION
IC	void CSTimeSmartPtr::swap		(self_type & rhs)
{
	object_type		*tmp = m_object; 
	m_object		= rhs.m_object; 
	rhs.m_object	= tmp;	
}

TEMPLATE_SPECIALIZATION
IC	bool CSTimeSmartPtr::equal		(const self_type & rhs)	const
{
	return			(m_object == rhs.m_object);
}

TEMPLATE_SPECIALIZATION
IC	void CSTimeSmartPtr::set		(object_type* rhs)
{
	m_object		= rhs;
	m_ref_count		= new u32(1);
	m_last_time_dec	= new u32(0);
}

TEMPLATE_SPECIALIZATION
IC	void CSTimeSmartPtr::set		(self_type const &rhs)
{
	if (!rhs.m_object)
		return;
	m_object		= rhs.m_object;
	m_ref_count		= rhs.m_ref_count;
	m_last_time_dec	= rhs.m_last_time_dec;
	++*m_ref_count;
}

TEMPLATE_SPECIALIZATION
IC	const typename CSTimeSmartPtr::object_type* CSTimeSmartPtr::get	()	const
{
	return			(m_object);
}

TEMPLATE_SPECIALIZATION
IC bool operator==					(CSTimeSmartPtr const &a, CSTimeSmartPtr const &b)		
{
	return			(a.get() == b.get());
}

TEMPLATE_SPECIALIZATION
IC bool operator	!=				(CSTimeSmartPtr const &a, CSTimeSmartPtr const &b)
{
	return			(a.get() != b.get());
}

TEMPLATE_SPECIALIZATION
IC bool operator<					(CSTimeSmartPtr const &a, CSTimeSmartPtr const &b)
{
	return			(a.get() <  b.get());
}

TEMPLATE_SPECIALIZATION
IC bool operator>					(CSTimeSmartPtr const &a, CSTimeSmartPtr const &b)
{
	return			(a.get() >  b.get());
}

TEMPLATE_SPECIALIZATION
IC void swap						(CSTimeSmartPtr &lhs, CSTimeSmartPtr &rhs)
{
	lhs.swap		(rhs);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSTimeSmartPtr
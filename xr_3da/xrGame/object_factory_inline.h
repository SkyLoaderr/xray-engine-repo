////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectFactory::CObjectItemAbstract::CObjectItemAbstract	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	m_clsid				(clsid)
#ifndef _EDITOR
	,m_script_clsid		(script_clsid)
#endif
{
}

IC	const CLASS_ID &CObjectFactory::CObjectItemAbstract::clsid	() const
{
	return				(m_clsid);
}

#ifndef _EDITOR
IC	ref_str	CObjectFactory::CObjectItemAbstract::script_clsid	() const
{
	return				(m_script_clsid);
}
#endif

IC	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const
{
	return				(item1->clsid() < item2->clsid());
}

IC	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item, const CLASS_ID &clsid) const
{
	return				(item->clsid() < clsid);
}

IC	CObjectFactory::CObjectItemPredicateCLSID::CObjectItemPredicateCLSID	(const CLASS_ID &clsid) :
	m_clsid				(clsid)
{
}

IC	bool CObjectFactory::CObjectItemPredicateCLSID::operator()	(const CObjectItemAbstract *item) const
{
	return				(m_clsid == item->clsid());
}

#ifndef _EDITOR
IC	CObjectFactory::CObjectItemPredicateScript::CObjectItemPredicateScript	(const ref_str &script_clsid_name) :
	m_script_clsid_name	(script_clsid_name)
{
}

IC	bool CObjectFactory::CObjectItemPredicateScript::operator()	(const CObjectItemAbstract *item) const
{
	return				(m_script_clsid_name == item->script_clsid());
}
#endif

template <typename _client_type, typename _server_type>
IC	CObjectFactory::CObjectItemCS<_client_type,_server_type>::CObjectItemCS	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	inherited			(clsid,script_clsid)
{
}

#ifndef _EDITOR
template <typename _client_type, typename _server_type>
CObjectFactory::CLIENT_BASE_CLASS *CObjectFactory::CObjectItemCS<_client_type,_server_type>::client_object	() const
{
	return				(xr_new<CLIENT_TYPE>());
}
#endif

template <typename _client_type, typename _server_type>
CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::CObjectItemCS<_client_type,_server_type>::server_object	(LPCSTR section) const
{
	return				(xr_new<SERVER_TYPE>(section));
}

template <typename _unknown_type, bool _client_object>
IC	CObjectFactory::CObjectItem<_unknown_type,_client_object>::CObjectItem	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	inherited			(clsid,script_clsid)
{
}

#ifndef _EDITOR
template <typename _unknown_type, bool _client_object>
CObjectFactory::CLIENT_BASE_CLASS *CObjectFactory::CObjectItem<_unknown_type,_client_object>::client_object	() const
{
	NODEFAULT;
#ifdef DEBUG
	return				(0);
#endif
}
#endif

template <typename _unknown_type, bool _client_object>
CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::CObjectItem<_unknown_type,_client_object>::server_object	(LPCSTR section) const
{
	return				(xr_new<SERVER_TYPE>(section));
}

#ifndef _EDITOR
template <typename _unknown_type>
IC	CObjectFactory::CObjectItem<_unknown_type,true>::CObjectItem	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	inherited			(clsid,script_clsid)
{
}

template <typename _unknown_type>
CObjectFactory::CLIENT_BASE_CLASS *CObjectFactory::CObjectItem<_unknown_type,true>::client_object	() const
{
	return				(xr_new<CLIENT_TYPE>());
}

template <typename _unknown_type>
CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::CObjectItem<_unknown_type,true>::server_object	(LPCSTR section) const
{
	NODEFAULT;
#ifdef DEBUG
	return				(0);
#endif
}
#endif

IC	const CObjectFactory::OBJECT_ITEM_STORAGE &CObjectFactory::clsids	() const
{
	return				(m_clsids);
}

#ifndef _EDITOR
IC	const CObjectFactory::CObjectItemAbstract &CObjectFactory::item	(const CLASS_ID &clsid) const
{
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,CObjectItemPredicate());
	VERIFY				((I != clsids().end()) && ((*I)->clsid() == clsid));
	return				(**I);
}
#else
IC	const CObjectFactory::CObjectItemAbstract *CObjectFactory::item	(const CLASS_ID &clsid, bool no_assert) const
{
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,CObjectItemPredicate());
	if ((I == clsids().end()) || ((*I)->clsid() != clsid)) {
		R_ASSERT		(no_assert);
		return			(0);
	}
	return				(*I);
}
#endif

#ifndef _EDITOR
template <typename _client_type, typename _server_type>
IC	void CObjectFactory::add	(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	{
		typedef boost::is_base_and_derived<CLIENT_BASE_CLASS,CType<_client_type,_server_type,CLIENT_BASE_CLASS>::type> a;
		STATIC_CHECK(a::value,Client_class_must_be_derived_from_the_CLIENT_BASE_CLASS);
	}
	{
		typedef boost::is_base_and_derived<SERVER_BASE_CLASS,CType<_client_type,_server_type,SERVER_BASE_CLASS>::type> a;
		STATIC_CHECK(a::value,Server_class_must_be_derived_from_the_SERVER_BASE_CLASS);
	}
	
	add					(xr_new<CObjectItemCS<CType<_client_type,_server_type,CLIENT_BASE_CLASS>::type,CType<_client_type,_server_type,SERVER_BASE_CLASS>::type> >(clsid,script_clsid));
}

template <typename _unknown_type>
IC	void CObjectFactory::add	(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	{
		typedef boost::is_base_and_derived<CLIENT_BASE_CLASS,_unknown_type> a;
		typedef boost::is_base_and_derived<SERVER_BASE_CLASS,_unknown_type> b;
		STATIC_CHECK	(a::value || b::value,Class_must_be_derived_from_the_CLIENT_BASE_CLASS_or_SERVER_BASE_CLASS);
	}
	add					(
		xr_new<
			CObjectItem<
				_unknown_type,
				boost::is_base_and_derived<CLIENT_BASE_CLASS,_unknown_type>::value
			>
		>
		(clsid,script_clsid)
	);
}
#else
template <typename _unknown_type>
IC	void CObjectFactory::add	(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	add					(xr_new<CObjectItem<_unknown_type,false> >(clsid,script_clsid));
}
#endif

IC	void CObjectFactory::add	(CObjectItemAbstract *item)
{
	R_ASSERT			(!m_initialized);
	
	const_iterator		I;
	
	I					= std::find_if(clsids().begin(),clsids().end(),CObjectItemPredicateCLSID(item->clsid()));
	VERIFY				(I == clsids().end());
	
#ifndef _EDITOR
	I					= std::find_if(clsids().begin(),clsids().end(),CObjectItemPredicateScript(item->script_clsid()));
	VERIFY				(I == clsids().end());
#endif
	
	m_clsids.push_back	(item);
}

#ifndef _EDITOR
IC	int	CObjectFactory::script_clsid	(const CLASS_ID &clsid) const
{
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,CObjectItemPredicate());
	VERIFY				((I != clsids().end()) && ((*I)->clsid() == clsid));
	return				(int(I - clsids().begin()));
}

IC	CObjectFactory::CLIENT_BASE_CLASS *CObjectFactory::client_object	(const CLASS_ID &clsid) const
{
	return				(item(clsid).client_object());
}

IC	CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::server_object	(const CLASS_ID &clsid, LPCSTR section) const
{
	return				(item(clsid).server_object(section));
}
#else
IC	CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::server_object	(const CLASS_ID &clsid, LPCSTR section) const
{
	const CObjectItemAbstract	*object = item(clsid,true);
	return				(object ? object->server_object(section) : 0);
}
#endif

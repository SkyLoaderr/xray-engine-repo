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

IC	CObjectFactory::CObjectItemPredicate::CObjectItemPredicate	(const CLASS_ID &clsid) :
	m_clsid				(clsid)
{
}

IC	CObjectFactory::CObjectItemPredicate::CObjectItemPredicate	() :
	m_clsid				(CLASS_ID(-1))
{
}

IC	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item) const
{
	return				(m_clsid == item->clsid());
}

IC	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const
{
	return				(item1->clsid() < item2->clsid());
}

IC	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item, const CLASS_ID &clsid) const
{
	return				(item->clsid() < clsid);
}

template <typename _client_type, typename _server_type>
IC	CObjectFactory::CObjectItem<_client_type,_server_type>::CObjectItem	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	inherited			(clsid,script_clsid)
{
}

#ifndef _EDITOR
template <typename _client_type, typename _server_type>
CObjectFactory::CLIENT_BASE_CLASS *CObjectFactory::CObjectItem<_client_type,_server_type>::client_object	() const
{
	return				(xr_new<CLIENT_TYPE>());
}
#endif

template <typename _client_type, typename _server_type>
CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::CObjectItem<_client_type,_server_type>::server_object	(LPCSTR section) const
{
	return				(xr_new<SERVER_TYPE>(section));
}

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
	if ((I != clsids().end()) && ((*I)->clsid() == clsid)) {
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
	add					(xr_new<CObjectItem<_client_type,_server_type> >(clsid,script_clsid));
}

template <typename _unknown_type>
IC	void CObjectFactory::add	(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	add					(
		xr_new<
			CObjectItem<
				CType<
					CLIENT_BASE_CLASS,
					_unknown_type,
					CClientClassInvalid
				>::type,
				CType<
					SERVER_BASE_CLASS,
					_unknown_type,
					CServerClassInvalid
				>::type> 
		>
		(clsid,script_clsid)
	);
}
#else
template <typename _unknown_type>
IC	void CObjectFactory::add	(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	add					(xr_new<CObjectItem<_unknown_type,_unknown_type> >(clsid,script_clsid));
}
#endif

IC	void CObjectFactory::add	(CObjectItemAbstract *item)
{
	const_iterator		I = std::find_if(clsids().begin(),clsids().end(),CObjectItemPredicate(item->clsid()));
	VERIFY				(I == clsids().end());
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

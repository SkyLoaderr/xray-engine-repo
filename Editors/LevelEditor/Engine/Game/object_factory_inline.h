////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CCLSID_Holder::ICLSID_Item::ICLSID_Item					(const CLASS_ID &clsid, LPCSTR script_clsid) :
	m_clsid				(clsid)
#ifndef _EDITOR
	,m_script_clsid		(script_clsid)
#endif
{
}

IC	CLASS_ID CCLSID_Holder::ICLSID_Item::clsid				() const
{
	return				(m_clsid);
}

#ifndef _EDITOR
IC	ref_str	CCLSID_Holder::ICLSID_Item::script_clsid		() const
{
	return				(m_script_clsid);
}
#endif

IC	CCLSID_Holder::ICLSID_ItemPredicate::ICLSID_ItemPredicate	(const CLASS_ID &clsid) :
	m_clsid				(clsid)
{
}

IC	CCLSID_Holder::ICLSID_ItemPredicate::ICLSID_ItemPredicate	() :
	m_clsid				(CLASS_ID(-1))
{
}

IC	bool CCLSID_Holder::ICLSID_ItemPredicate::operator()	(const ICLSID_Item *item) const
{
	return				(m_clsid == item->clsid());
}

IC	bool CCLSID_Holder::ICLSID_ItemPredicate::operator()	(const ICLSID_Item *item1, const ICLSID_Item *item2) const
{
	return				(item1->clsid() < item2->clsid());
}

IC	bool CCLSID_Holder::ICLSID_ItemPredicate::operator()	(const ICLSID_Item *item, const CLASS_ID &clsid) const
{
	return				(item->clsid() < clsid);
}

template <typename _client_type, typename _server_type>
IC	CCLSID_Holder::CCLSID_Item<_client_type,_server_type>::CCLSID_Item				(const CLASS_ID &clsid, LPCSTR script_clsid) :
	inherited			(clsid,script_clsid)
{
}

#ifndef _EDITOR
template <typename _client_type, typename _server_type>
DLL_Pure *CCLSID_Holder::CCLSID_Item<_client_type,_server_type>::client_object		() const
{
	return				(xr_new<CLIENT_TYPE>());
}
#endif

template <typename _client_type, typename _server_type>
CSE_Abstract *CCLSID_Holder::CCLSID_Item<_client_type,_server_type>::server_object	(LPCSTR section) const
{
	return				(xr_new<SERVER_TYPE>(section));
}

IC	const CCLSID_Holder::CLSID_STORAGE &CCLSID_Holder::clsids	() const
{
	return				(m_clsids);
}

#ifndef _EDITOR
IC	const CCLSID_Holder::ICLSID_Item &CCLSID_Holder::item		(const CLASS_ID &clsid) const
{
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,ICLSID_ItemPredicate());
	VERIFY				((I != clsids().end()) && ((*I)->clsid() == clsid));
	return				(**I);
}
#else
IC	const CCLSID_Holder::ICLSID_Item *CCLSID_Holder::item		(const CLASS_ID &clsid, bool no_assert) const
{
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,ICLSID_ItemPredicate());
	if ((I != clsids().end()) && ((*I)->clsid() == clsid)) {
		R_ASSERT		(no_assert);
		return			(0);
	}
	return				(*I);
}
#endif

#ifndef _EDITOR
template <typename _client_type, typename _server_type>
IC	void CCLSID_Holder::add									(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	add					(xr_new<CCLSID_Item<_client_type,_server_type> >(clsid,script_clsid));
}

template <typename _unknown_type>
IC	void CCLSID_Holder::add									(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	add					(
		xr_new<
			CCLSID_Item<
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
IC	void CCLSID_Holder::add									(const CLASS_ID &clsid, LPCSTR script_clsid)
{
	add					(xr_new<CCLSID_Item<_unknown_type,_unknown_type> >(clsid,script_clsid));
}
#endif

IC	void CCLSID_Holder::add									(ICLSID_Item *item)
{
	const_iterator		I = std::find_if(clsids().begin(),clsids().end(),ICLSID_ItemPredicate(item->clsid()));
	VERIFY				(I == clsids().end());
	m_clsids.push_back	(item);
}

#ifndef _EDITOR
IC	ref_str	CCLSID_Holder::script_clsid						(const CLASS_ID &clsid) const
{
	return				(item(clsid).script_clsid());
}

IC	CCLSID_Holder::CLIENT_BASE_CLASS *CCLSID_Holder::client_object	(const CLASS_ID &clsid) const
{
	return				(item(clsid).client_object());
}

IC	CCLSID_Holder::SERVER_BASE_CLASS *CCLSID_Holder::server_object	(const CLASS_ID &clsid, LPCSTR section) const
{
	return				(item(clsid).server_object(section));
}
#else
IC	CCLSID_Holder::SERVER_BASE_CLASS *CCLSID_Holder::server_object	(const CLASS_ID &clsid, LPCSTR section) const
{
	const ICLSID_Item	*object = item(clsid,true);
	return				(object ? object->server_object(section) : 0);
}
#endif

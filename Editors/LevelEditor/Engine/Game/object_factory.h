////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _EDITOR
#include <boost/type_traits/is_base_and_derived.hpp>
#endif
#include "xrServer_Objects.h"

class CCLSID_Holder {
private:
	typedef DLL_Pure				CLIENT_BASE_CLASS;
	typedef CSE_Abstract			SERVER_BASE_CLASS;

private:
#ifndef _EDITOR
	struct CInternal{};

	struct CClientClassInvalid : public CLIENT_BASE_CLASS
	{
		IC							CClientClassInvalid	()
		{
			NODEFAULT;
		}
	};

	struct CServerClassInvalid : public SERVER_BASE_CLASS
	{
		IC							CServerClassInvalid	(LPCSTR section) :
			SERVER_BASE_CLASS		(section)
		{
			NODEFAULT;
		}

		virtual void				STATE_Write			(NET_Packet &tNetPacket){};
		virtual void				STATE_Read			(NET_Packet &tNetPacket, u16 size){};
		virtual void				UPDATE_Write		(NET_Packet &tNetPacket){};
		virtual void				UPDATE_Read			(NET_Packet &tNetPacket){};
	};
#endif

protected:
	class ICLSID_Item {
	protected:
		CLASS_ID					m_clsid;
#ifndef _EDITOR
		ref_str						m_script_clsid;
#endif

	public:
		IC							ICLSID_Item			(const CLASS_ID &clsid, LPCSTR script_clsid);
		IC		CLASS_ID			clsid				() const;
#ifndef _EDITOR
		IC		ref_str				script_clsid		() const;
		virtual CLIENT_BASE_CLASS	*client_object		() const = 0;
#endif
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const = 0;
	};

	template <typename _client_type, typename _server_type>
	class CCLSID_Item : public ICLSID_Item {
	protected:
		typedef ICLSID_Item			inherited;
#ifndef _EDITOR
		typedef _client_type		CLIENT_TYPE;
#endif
		typedef _server_type		SERVER_TYPE;

	public:
		IC							CCLSID_Item			(const CLASS_ID &clsid, LPCSTR script_clsid);
#ifndef _EDITOR
		virtual CLIENT_BASE_CLASS	*client_object		() const;
#endif
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
	};

	struct ICLSID_ItemPredicate {
		CLASS_ID					m_clsid;

		IC							ICLSID_ItemPredicate(const CLASS_ID &clsid);
		IC							ICLSID_ItemPredicate();
		IC	bool					operator()			(const ICLSID_Item *item) const;
		IC	bool					operator()			(const ICLSID_Item *item1, const ICLSID_Item *item2) const;
		IC	bool					operator()			(const ICLSID_Item *item1, const CLASS_ID &clsid) const;
	};

#ifndef _EDITOR
	template <typename _base, typename _derived, typename _default>
	struct CType {
		template <typename _derived, typename _default, bool value>
		struct CInternalType {
			typedef _default value;
		};

		template <typename _derived, typename _default>
		struct CInternalType<_derived,_default,true> {
			typedef _derived value;
		};

		typedef typename CInternalType<_derived,_default,boost::is_base_and_derived<_base,_derived>::value>::value type;
	};
#endif

public:
	typedef xr_vector<ICLSID_Item*>			CLSID_STORAGE;
	typedef CLSID_STORAGE::iterator			iterator;
	typedef CLSID_STORAGE::const_iterator	const_iterator;

protected:
	CLSID_STORAGE					m_clsids;

protected:
	IC		void					add					(ICLSID_Item *item);
	IC		const CLSID_STORAGE		&clsids				() const;
#ifndef _EDITOR
	IC		const ICLSID_Item		&item				(const CLASS_ID &clsid) const;
#else
	IC		const ICLSID_Item		*item				(const CLASS_ID &clsid, bool no_assert) const;
#endif

public:
									CCLSID_Holder		();
	virtual							~CCLSID_Holder		();
	template <typename _unknown_type>
	IC		void					add					(const CLASS_ID &clsid, LPCSTR script_clsid);
#ifndef _EDITOR
	template <typename _client_type, typename _server_type>
	IC		void					add					(const CLASS_ID &clsid, LPCSTR script_clsid);
	IC		ref_str					script_clsid		(const CLASS_ID &clsid) const;
			void					script_register		() const;
	IC		CLIENT_BASE_CLASS		*client_object		(const CLASS_ID &clsid) const;
	IC		SERVER_BASE_CLASS		*server_object		(const CLASS_ID &clsid, LPCSTR section) const;
#else
	IC		SERVER_BASE_CLASS		*server_object		(const CLASS_ID &clsid, LPCSTR section) const;
#endif
};

IC	const CCLSID_Holder &object_factory()
{
	static CCLSID_Holder object_factory;
	return	(object_factory);
}

#include "object_factory_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef XRGAME_EXPORTS
#	define NO_XR_GAME
#endif

#ifndef NO_XR_GAME
#	ifndef XRSE_FACTORY_AXPORTS
#		define NO_SCRIPTS
#	endif
#endif

#include "script_export_space.h"

#ifndef NO_XR_GAME
#	include <boost/type_traits/is_base_and_derived.hpp>
#	include "script_space.h"
#endif

#include "xrServer_Objects.h"

#ifndef NO_XR_GAME
	class CAttachableItem;
	class CSE_Abstract;
#endif


class CObjectFactory {
private:
#ifndef NO_XR_GAME
	typedef DLL_Pure				CLIENT_BASE_CLASS;
	typedef CAttachableItem			CLIENT_SCRIPT_BASE_CLASS;
	typedef CSE_Abstract			SERVER_SCRIPT_BASE_CLASS;
#endif
	typedef CSE_Abstract			SERVER_BASE_CLASS;

private:
#ifndef NO_XR_GAME
	struct CInternal{};
#endif

protected:
	class CObjectItemAbstract {
	protected:
		CLASS_ID					m_clsid;
#ifndef NO_XR_GAME
		ref_str						m_script_clsid;
#endif

	public:
		IC							CObjectItemAbstract	(const CLASS_ID &clsid, LPCSTR script_clsid);
		IC		const CLASS_ID		&clsid				() const;
#ifndef NO_XR_GAME
		IC		ref_str				script_clsid		() const;
		virtual CLIENT_BASE_CLASS	*client_object		() const = 0;
#endif
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const = 0;
	};

	template <typename _client_type, typename _server_type>
	class CObjectItemCS : public CObjectItemAbstract {
	protected:
		typedef CObjectItemAbstract	inherited;
		typedef _client_type		CLIENT_TYPE;
		typedef _server_type		SERVER_TYPE;

	public:
		IC							CObjectItemCS		(const CLASS_ID &clsid, LPCSTR script_clsid);
#ifndef NO_XR_GAME
		virtual CLIENT_BASE_CLASS	*client_object		() const;
#endif
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
	};

	template <typename _unknown_type, bool _client_type>
	class CObjectItem : public CObjectItemAbstract {
	protected:
		typedef CObjectItemAbstract	inherited;
		typedef _unknown_type		SERVER_TYPE;

	public:
		IC							CObjectItem			(const CLASS_ID &clsid, LPCSTR script_clsid);
#ifndef NO_XR_GAME
		virtual CLIENT_BASE_CLASS	*client_object		() const;
#endif
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
	};

#ifndef NO_XR_GAME
	template <typename _unknown_type>
	class CObjectItem<_unknown_type,true> : public CObjectItemAbstract {
	protected:
		typedef CObjectItemAbstract	inherited;
		typedef _unknown_type		CLIENT_TYPE;

	public:
		IC							CObjectItem			(const CLASS_ID &clsid, LPCSTR script_clsid);
		virtual CLIENT_BASE_CLASS	*client_object		() const;
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
	};

//	template <typename a, typename b, typename c>
//	struct CType {
//		template <bool value>
//		struct CInternalType {
//			typedef b type;
//		};
//
//		template <>
//		struct CInternalType<true> {
//			typedef a type;
//		};
//
//		typedef typename CInternalType<boost::is_base_and_derived<c,a>::value>::type type;
//	};
//

	class CObjectItemScript : public CObjectItemAbstract {
	protected:
		typedef CObjectItemAbstract	inherited;
		luabind::functor<void>		m_client_creator;
		luabind::functor<void>		m_server_creator;

	public:
		IC							CObjectItemScript	(luabind::functor<void> client_creator, luabind::functor<void> server_creator, const CLASS_ID &clsid, LPCSTR script_clsid);
		virtual CLIENT_BASE_CLASS	*client_object		() const;
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
	};

#endif


	struct CObjectItemPredicate {
		IC	bool					operator()			(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const;
		IC	bool					operator()			(const CObjectItemAbstract *item, const CLASS_ID &clsid) const;
	};

	struct CObjectItemPredicateCLSID {
		CLASS_ID					m_clsid;

		IC							CObjectItemPredicateCLSID	(const CLASS_ID &clsid);
		IC	bool					operator()					(const CObjectItemAbstract *item) const;
	};

#ifndef NO_XR_GAME
	struct CObjectItemPredicateScript {
		ref_str						m_script_clsid_name;

		IC							CObjectItemPredicateScript	(const ref_str &script_clsid_name);
		IC	bool					operator()					(const CObjectItemAbstract *item) const;
	};
#endif

public:
	typedef xr_vector<CObjectItemAbstract*>		OBJECT_ITEM_STORAGE;
	typedef OBJECT_ITEM_STORAGE::iterator		iterator;
	typedef OBJECT_ITEM_STORAGE::const_iterator	const_iterator;

protected:
	OBJECT_ITEM_STORAGE					m_clsids;
	bool								m_initialized;
#ifndef NO_XR_GAME
	mutable CLIENT_SCRIPT_BASE_CLASS	*m_client_instance;
	mutable SERVER_SCRIPT_BASE_CLASS	*m_server_instance;
#endif

protected:
			void						register_classes();
	IC		void						add				(CObjectItemAbstract *item);
	IC		const OBJECT_ITEM_STORAGE	&clsids			() const;
	template <typename _unknown_type>
	IC		void						add				(const CLASS_ID &clsid, LPCSTR script_clsid);
#ifndef NO_XR_GAME
	template <typename _client_type, typename _server_type>
	IC		void						add				(const CLASS_ID &clsid, LPCSTR script_clsid);
	IC		const CObjectItemAbstract	&item			(const CLASS_ID &clsid) const;
			CLIENT_SCRIPT_BASE_CLASS	*client_instance() const;
			SERVER_SCRIPT_BASE_CLASS	*server_instance() const;
#else
	IC		const CObjectItemAbstract	*item			(const CLASS_ID &clsid, bool no_assert) const;
#endif

public:
									CObjectFactory		();
	virtual							~CObjectFactory		();
#ifndef NO_XR_GAME
	IC		int						script_clsid		(const CLASS_ID &clsid) const;
			void					register_script		() const;
	IC		CLIENT_BASE_CLASS		*client_object		(const CLASS_ID &clsid) const;
	IC		SERVER_BASE_CLASS		*server_object		(const CLASS_ID &clsid, LPCSTR section) const;
			void					register_script_class(LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid);
			void					set_instance		(CLIENT_SCRIPT_BASE_CLASS *instance) const;
			void					set_instance		(SERVER_SCRIPT_BASE_CLASS *instance) const;
#else
	IC		SERVER_BASE_CLASS		*server_object		(const CLASS_ID &clsid, LPCSTR section) const;
#endif
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CObjectFactory)
#undef script_type_list
#define script_type_list save_type_list(CObjectFactory)

IC	const CObjectFactory &object_factory()
{
	static CObjectFactory object_factory;
	return	(object_factory);
}

#include "object_factory_inline.h"
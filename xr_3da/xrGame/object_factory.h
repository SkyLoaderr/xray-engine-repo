////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"
#include "object_item_abstract.h"
#include "xrServer_Objects_Abstract.h"

class CObjectFactory {
public:
#ifndef NO_XR_GAME
	typedef ObjectFactory::CLIENT_BASE_CLASS			CLIENT_BASE_CLASS;
#endif
	typedef ObjectFactory::SERVER_BASE_CLASS			SERVER_BASE_CLASS;

#ifndef NO_SCRIPTS
#	ifndef NO_XR_GAME
		typedef ObjectFactory::CLIENT_SCRIPT_BASE_CLASS	CLIENT_SCRIPT_BASE_CLASS;
#	endif
		typedef ObjectFactory::SERVER_SCRIPT_BASE_CLASS	SERVER_SCRIPT_BASE_CLASS;
#endif

protected:
	struct CObjectItemPredicate {
		IC	bool					operator()			(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const;
		IC	bool					operator()			(const CObjectItemAbstract *item, const CLASS_ID &clsid) const;
	};

	struct CObjectItemPredicateCLSID {
		CLASS_ID					m_clsid;

		IC							CObjectItemPredicateCLSID	(const CLASS_ID &clsid);
		IC	bool					operator()					(const CObjectItemAbstract *item) const;
	};

#ifndef NO_SCRIPTS
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
#ifndef NO_SCRIPTS
#	ifndef NO_XR_GAME
		mutable CLIENT_SCRIPT_BASE_CLASS*m_client_instance;
#	endif
	mutable SERVER_SCRIPT_BASE_CLASS	*m_server_instance;
#endif

protected:
			void						register_classes		();
	IC		void						add						(CObjectItemAbstract *item);
	IC		const OBJECT_ITEM_STORAGE	&clsids					() const;
	template <typename _unknown_type>
	IC		void						add						(const CLASS_ID &clsid, LPCSTR script_clsid);

#ifndef NO_XR_GAME
	template <typename _client_type, typename _server_type>
	IC		void						add						(const CLASS_ID &clsid, LPCSTR script_clsid);
	IC		const CObjectItemAbstract	&item					(const CLASS_ID &clsid) const;
#else
	IC		const CObjectItemAbstract	*item					(const CLASS_ID &clsid, bool no_assert) const;
#endif

public:
										CObjectFactory			();
	virtual								~CObjectFactory			();

#ifndef NO_XR_GAME
	IC		CLIENT_BASE_CLASS			*client_object			(const CLASS_ID &clsid) const;
	IC		SERVER_BASE_CLASS			*server_object			(const CLASS_ID &clsid, LPCSTR section) const;
#else
	IC		SERVER_BASE_CLASS			*server_object			(const CLASS_ID &clsid, LPCSTR section) const;
#endif

#ifndef NO_SCRIPTS
	IC		int							script_clsid			(const CLASS_ID &clsid) const;
			void						register_script			() const;
			void						register_script_class	(LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid);
			void						register_script_classes	();
#ifndef NO_XR_GAME
	IC		void						set_instance			(CLIENT_SCRIPT_BASE_CLASS *instance) const;
	IC		CLIENT_SCRIPT_BASE_CLASS	*client_instance		() const;
#endif
	IC		void						set_instance			(SERVER_SCRIPT_BASE_CLASS *instance) const;
	IC		SERVER_SCRIPT_BASE_CLASS	*server_instance		() const;
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
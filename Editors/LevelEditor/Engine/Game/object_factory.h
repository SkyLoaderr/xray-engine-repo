////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _EDITOR
#	include <boost/type_traits/is_base_and_derived.hpp>
#endif

#include "xrServer_Objects.h"

class CObjectFactory {
private:
#ifndef _EDITOR
	typedef DLL_Pure				CLIENT_BASE_CLASS;
#endif
	typedef CSE_Abstract			SERVER_BASE_CLASS;

private:
#ifndef _EDITOR
	struct CInternal{};
#endif

protected:
	class CObjectItemAbstract {
	protected:
		CLASS_ID					m_clsid;
#ifndef _EDITOR
		ref_str						m_script_clsid;
#endif

	public:
		IC							CObjectItemAbstract	(const CLASS_ID &clsid, LPCSTR script_clsid);
		IC		const CLASS_ID		&clsid				() const;
#ifndef _EDITOR
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
#ifndef _EDITOR
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
#ifndef _EDITOR
		virtual CLIENT_BASE_CLASS	*client_object		() const;
#endif
		virtual SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
	};

#ifndef _EDITOR
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

    template<bool>
    struct CompileTimeError;

    template<>
    struct CompileTimeError<true>
    {
    };

	template <typename a, typename b, typename c>
	struct CType {
		template <bool value>
		struct CInternalType {
			typedef b type;
		};

		template <>
		struct CInternalType<true> {
			typedef a type;
		};

		typedef typename CInternalType<boost::is_base_and_derived<c,a>::value>::type type;
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

#ifndef _EDITOR
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
	OBJECT_ITEM_STORAGE				m_clsids;
	bool							m_initialized;

protected:
			void						register_classes();
	IC		void						add				(CObjectItemAbstract *item);
	IC		const OBJECT_ITEM_STORAGE	&clsids			() const;
	template <typename _unknown_type>
	IC		void						add				(const CLASS_ID &clsid, LPCSTR script_clsid);
#ifndef _EDITOR
	template <typename _client_type, typename _server_type>
	IC		void						add				(const CLASS_ID &clsid, LPCSTR script_clsid);
	IC		const CObjectItemAbstract	&item			(const CLASS_ID &clsid) const;
#else
	IC		const CObjectItemAbstract	*item			(const CLASS_ID &clsid, bool no_assert) const;
#endif

public:
									CObjectFactory		();
	virtual							~CObjectFactory		();
#ifndef _EDITOR
	IC		int						script_clsid		(const CLASS_ID &clsid) const;
			void					register_script		() const;
	IC		CLIENT_BASE_CLASS		*client_object		(const CLASS_ID &clsid) const;
	IC		SERVER_BASE_CLASS		*server_object		(const CLASS_ID &clsid, LPCSTR section) const;
#else
	IC		SERVER_BASE_CLASS		*server_object		(const CLASS_ID &clsid, LPCSTR section) const;
#endif
};

IC	const CObjectFactory &object_factory()
{
	static CObjectFactory object_factory;
	return	(object_factory);
}

#ifndef _EDITOR

#ifdef STATIC_CHECK
#	undef STATIC_CHECK
#endif

#define STATIC_CHECK(expr, msg) \
{ \
        CompileTimeError<((expr) != 0)> ERROR_##msg; \
        (void)ERROR_##msg; \
}
#endif

#include "object_factory_inline.h"

#ifndef _EDITOR
#	undef STATIC_CHECK
#endif

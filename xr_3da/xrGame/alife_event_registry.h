////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_event.h"

class CALifeEventRegistry {
protected:
	template <typename _predicate>
	struct CEventIDPredicate {
		const _predicate			&m_predicate;

		IC							CEventIDPredicate	(const _predicate &predicate) :
										m_predicate		(predicate)
		{
		}

		IC const ALife::_EVENT_ID	operator()			(CALifeEvent *T) const
		{
			m_predicate				(T);
			return					(T->m_tEventID);
		}

		IC CEventIDPredicate<_predicate> &operator=		(const CEventIDPredicate<_predicate> &T)
		{
			*this					= T;
			return					(*this);
		}
	};

protected:
	ALife::_EVENT_ID				m_id;
	ALife::EVENT_MAP				m_events;

public:
	IC								CALifeEventRegistry		(LPCSTR section);
	virtual							~CALifeEventRegistry	();
	virtual	void					save					(IWriter &memory_stream);
	template <typename _predicate>
	IC		void					load					(IReader &file_stream, const _predicate &predicate);
	IC		void					add						(CALifeEvent	*event);
	IC		void					remove					(const ALife::_EVENT_ID	&event_id, bool no_assert = false);
	IC		CALifeEvent				*event					(const ALife::_EVENT_ID	&event_id, bool no_assert = false) const;
	IC		const ALife::EVENT_MAP	&events					() const;
};

#include "alife_event_registry_inline.h"
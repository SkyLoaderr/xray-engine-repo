////////////////////////////////////////////////////////////////////////////
//	Module 		: greeting_manager.h
//	Created 	: 09.12.2004
//  Modified 	: 09.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Greeting manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_manager.h"
#include "ai/stalker/ai_stalker.h"

class CGreetingManager : public CObjectManager<const CAI_Stalker> {
public:
	typedef CObjectManager<const CAI_Stalker>	inherited;

public:
	struct CGreeting {
		const CAI_Stalker	*m_object;
		u32					m_time;

		IC		CGreeting	(const CAI_Stalker *stalker, u32 time)
		{
			VERIFY			(stalker);
			m_object		= stalker;
			m_time			= time;
		}

		IC	bool operator==	(const CAI_Stalker *stalker) const;
	};

public:
	typedef xr_vector<CGreeting>				GREETINGS;

protected:
	CAI_Stalker				*m_object;
	GREETINGS				m_greetings;

protected:
	IC		const CGreeting	*old_greeting		(const CAI_Stalker *stalker) const;
	IC		void			remove_old_greetings();

public:
							CGreetingManager	(CCustomMonster *object);
	virtual void			reload				(LPCSTR section);
	virtual bool			useful				(const CAI_Stalker *object) const;
	virtual	float			evaluate			(const CAI_Stalker *object) const;
	virtual void			update				();
			void			process_greeting	();
};

#include "greeting_manager_inline.h"
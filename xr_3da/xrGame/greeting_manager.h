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
protected:
	typedef CObjectManager<const CAI_Stalker>	inherited;
	typedef xr_vector<const CAI_Stalker*>		GREETINGS;

protected:
	CAI_Stalker				*m_object;

public:
							CGreetingManager	(CCustomMonster *object);
	virtual void			reload				(LPCSTR section);
	virtual bool			useful				(const CAI_Stalker *object) const;
	virtual	float			evaluate			(const CAI_Stalker *object) const;
	virtual void			update				();

public:
	IC		const GREETINGS	&greetings			() const;
};

#include "greeting_manager_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAI_Stalker;

class CAgentManager : ISheduled {
protected:
	typedef ISheduled						inherited;
	typedef xr_vector<CAI_Stalker*>			MEMBER_STORAGE;
	typedef MEMBER_STORAGE::iterator		iterator;
	typedef MEMBER_STORAGE::const_iterator	const_iterator;

protected:
	MEMBER_STORAGE			m_members;

public:
							CAgentManager	();
	virtual					~CAgentManager	();
	virtual float			shedule_Scale	();
	virtual void			shedule_Update	(u32 time_delta);	
	virtual BOOL			shedule_Ready	();
			void			add				(CAI_Stalker *member);
			void			remove			(CAI_Stalker *member);
			void			update			(u32 time_delta);
};

#include "agent_manager_inline.h"
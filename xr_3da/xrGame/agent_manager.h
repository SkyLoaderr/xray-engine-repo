////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager.h"

class CEntity;
class CAI_Stalker;

class CAgentManager :
	public CMotivationActionManager<CAgentManager>,
	public ISheduled
{
protected:
	typedef CMotivationActionManager<CAgentManager>	inherited;
	typedef xr_vector<CAI_Stalker*>					MEMBER_STORAGE;
	typedef MEMBER_STORAGE::iterator				iterator;
	typedef MEMBER_STORAGE::const_iterator			const_iterator;

protected:
	MEMBER_STORAGE			m_members;

protected:
			void			add_motivations	();
			void			add_evaluators	();
			void			add_actions		();

public:
							CAgentManager	();
	virtual					~CAgentManager	();
	virtual float			shedule_Scale	();
	virtual void			shedule_Update	(u32 time_delta);	
	virtual BOOL			shedule_Ready	();
	virtual void			reload			(LPCSTR section);	
			void			add				(CEntity *member);
			void			remove			(CEntity *member);
};

#include "agent_manager_inline.h"
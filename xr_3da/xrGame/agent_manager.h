////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager.h"
#include "member_order.h"

class CEntity;
class CAI_Stalker;
class CSetupAction;

class CAgentManager :
	public CMotivationActionManager<CAgentManager>,
	public ISheduled
{
public:
	class CMemberPredicate {
	protected:
		CAI_Stalker		*m_object;

	public:
		IC				CMemberPredicate	(CAI_Stalker *object) :
							m_object	(object)
		{
		}

		IC		bool	operator()			(const CMemberOrder &order) const
		{
			return			(order.object() == m_object);
		}
	};

protected:
	typedef CMotivationActionManager<CAgentManager>	inherited;
	typedef xr_vector<CMemberOrder>					MEMBER_STORAGE;
	typedef MEMBER_STORAGE::iterator				iterator;
	typedef MEMBER_STORAGE::const_iterator			const_iterator;
	
	using inherited::add_condition;

protected:
	MEMBER_STORAGE					m_members;

protected:
			void					add_motivations	();
			void					add_evaluators	();
			void					add_actions		();

public:
									CAgentManager	();
	virtual							~CAgentManager	();
	virtual float					shedule_Scale	();
	virtual void					shedule_Update	(u32 time_delta);	
	virtual BOOL					shedule_Ready	();
	virtual void					reload			(LPCSTR section);	
			void					add				(CEntity *member);
			void					remove			(CEntity *member);
	IC		const CSetupAction		&action			(CAI_Stalker *object) const;
	IC		const CMemberOrder		&member			(CAI_Stalker *object) const;
	IC		const MEMBER_STORAGE	&members		() const;
};

#include "agent_manager_inline.h"
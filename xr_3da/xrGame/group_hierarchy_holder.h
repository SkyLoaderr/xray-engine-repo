////////////////////////////////////////////////////////////////////////////
//	Module 		: group_hierarchy_holder.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Group hierarchy holder
////////////////////////////////////////////////////////////////////////////

#pragma once

class CEntity;
class CAgentManager;
class CSquadHierarchyHolder;

namespace GroupHierarchyHolder {
	typedef xr_vector<CEntity*> MEMBER_REGISTRY;
}

class CGroupHierarchyHolder {
private:
	typedef GroupHierarchyHolder::MEMBER_REGISTRY MEMBER_REGISTRY;

private:
	CSquadHierarchyHolder			*m_squad;
	MEMBER_REGISTRY					m_members;
	CEntity							*m_leader;

	// TODO: for stalker only, should be removed
private:
	CAgentManager					*m_agent_manager;

	// TODO: for rats only, should be removed
public:
	u32								m_dwLastActionTime;
	u32								m_dwLastAction;
	u32								m_dwActiveCount;
	u32								m_dwAliveCount;
	u32								m_dwStandingCount;

private:
	IC		CAgentManager			*get_agent_manager		() const;

public:
	IC								CGroupHierarchyHolder	(CSquadHierarchyHolder *squad);
	virtual							~CGroupHierarchyHolder	();
	IC		CAgentManager			&agent_manager			() const;
	IC		const MEMBER_REGISTRY	&members	() const;
			void					register_member			(CEntity *member);
			void					unregister_member		(CEntity *member);
	IC		CSquadHierarchyHolder	&squad					() const;
	IC		CEntity					*leader					() const;
			void					update_leader			();
};

#include "group_hierarchy_holder_inline.h"
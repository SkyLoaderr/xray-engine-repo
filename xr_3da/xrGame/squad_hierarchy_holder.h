////////////////////////////////////////////////////////////////////////////
//	Module 		: squad_hierarchy_holder.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Squad hierarchy holder
////////////////////////////////////////////////////////////////////////////

#pragma once

class CGroupHierarchyHolder;
class CEntity;
class CTeamHierarchyHolder;

namespace MemorySpace {
	struct CVisibleObject;
	struct CSoundObject;
	struct CHitObject;
}

namespace SquadHierarchyHolder {
	typedef xr_vector<MemorySpace::CVisibleObject>	VISIBLE_OBJECTS;
	typedef xr_vector<MemorySpace::CSoundObject>	SOUND_OBJECTS;
	typedef xr_vector<MemorySpace::CHitObject>		HIT_OBJECTS;
	typedef xr_vector<CGroupHierarchyHolder*>		GROUP_REGISTRY;
}

class CSquadHierarchyHolder {
private:
	enum {max_group_count = 32};

private:
	typedef SquadHierarchyHolder::GROUP_REGISTRY	GROUP_REGISTRY;
	typedef SquadHierarchyHolder::VISIBLE_OBJECTS	VISIBLE_OBJECTS;
	typedef SquadHierarchyHolder::SOUND_OBJECTS		SOUND_OBJECTS;
	typedef SquadHierarchyHolder::HIT_OBJECTS		HIT_OBJECTS;

private:
	u32								m_member_count;
	CTeamHierarchyHolder			*m_team;
	CEntity							*m_leader;
	mutable GROUP_REGISTRY			m_groups;
	VISIBLE_OBJECTS					*m_visible_objects;
	SOUND_OBJECTS					*m_sound_objects;
	HIT_OBJECTS						*m_hit_objects;

public:
	IC								CSquadHierarchyHolder	(CTeamHierarchyHolder *team);
	virtual							~CSquadHierarchyHolder	();
			CGroupHierarchyHolder	&group					(u32 group_id) const;
	IC		CEntity					*leader					() const;
	IC		CTeamHierarchyHolder	&team					() const;
			void					dec_member_count		();
			void					inc_member_count		();
	IC		u32						member_count			() const;
	IC		void					leader					(CEntity *leader);
	IC		VISIBLE_OBJECTS			&visible_objects		() const;
	IC		SOUND_OBJECTS			&sound_objects			() const;
	IC		HIT_OBJECTS				&hit_objects			() const;
			void					update_leader			();
	IC		const GROUP_REGISTRY	&groups					() const;
};

#include "squad_hierarchy_holder_inline.h"
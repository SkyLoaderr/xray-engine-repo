////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap.h
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"
#include "inventoryowner.h"
#include "action_planner.h"

class CAI_Stalker;
class CWeapon;

class CObjectHandlerGOAP : 
	public CActionPlanner<CAI_Stalker>,
	public CInventoryOwner

{
protected:
	typedef CActionPlanner<CAI_Stalker>	inherited;

protected:
	bool						m_bHammerIsClutched;

private:
	// firing
	bool						m_bFiring;
	u32							m_dwNoFireTime;
	u32							m_dwStartFireTime;
	bool						m_bCanFire;

	// fire  constants
	u32							m_dwFireRandomMin;
	u32							m_dwFireRandomMax;
	u32							m_dwNoFireTimeMin;
	u32							m_dwNoFireTimeMax;
	float						m_fMinMissDistance;
	float						m_fMinMissFactor;
	float						m_fMaxMissDistance;
	float						m_fMaxMissFactor;
	MonsterSpace::EObjectAction	m_tWeaponState;

public:
							CObjectHandlerGOAP		();
	virtual					~CObjectHandlerGOAP		();
			void			init					();
	virtual	void			Load					(LPCSTR section);
	virtual	void			reinit					(CAI_Stalker *object);
	virtual	void			reload					(LPCSTR section);
	virtual	void			update					(u32 time_delta);
	virtual void			OnItemTake				(CInventoryItem *inventory_item);
	virtual void			OnItemDrop				(CInventoryItem *inventory_item);
	virtual void			OnItemDropUpdate		();
	IC		bool			firing					() const;
	IC		u32				uid						(const u32 id0, const u32 id1) const;
	IC		bool			object_action			(u32 action_id, CObject *object);
	IC		u32				current_action_object_id() const;
	IC		u32				current_action_state_id	() const;
	IC		bool			goal_reached			() const;
			CInventoryItem	*best_weapon			() const;
			u32				weapon_state			(const CWeapon *weapon) const;
			u32				object_action			() const;
			void			add_item				(CInventoryItem *inventory_item);
			void			remove_item				(CInventoryItem *inventory_item);
			void			set_goal				(const MonsterSpace::EObjectAction object_action, CGameObject *game_object = 0);
};

#include "object_handler_goap_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.h
//	Created 	: 02.10.2001
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"
#include "state_manager_state.h"
#include "inventoryowner.h"

class CAI_Stalker;
class CWeapon;

class CObjectHandler : 
	public CStateManagerState<
		CAI_Stalker
	>,
	public CInventoryOwner

{
	typedef CStateManagerState<CAI_Stalker> inherited;
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
					CObjectHandler			();
	virtual			~CObjectHandler			();
			void	init					();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					(CAI_Stalker *object);
	virtual	void	reload					(LPCSTR section);
	virtual	void	update					(u32 time_delta);
	virtual	bool	check_if_can_kill_enemy	(const SRotation &orientation) const;
	virtual	bool	check_if_can_kill_member() const;
	virtual	bool	check_if_can_kill_target(const CEntity *tpEntity, const Fvector &target_pos, float yaw2, float pitch2, float fSafetyAngle = FIRE_SAFETY_ANGLE) const;
	virtual void	OnItemTake				(CInventoryItem *inventory_item);
	virtual void	OnItemDrop				(CInventoryItem *inventory_item);
	virtual void	OnItemDropUpdate		();
	IC		bool	firing					() const;
	IC		u32		uid						(const u32 id0, const u32 id1) const;
			void	add_item				(CInventoryItem *inventory_item);
			void	remove_item				(CInventoryItem *inventory_item);
			void	set_dest_state			(const MonsterSpace::EObjectAction object_action, CGameObject *game_object = 0);
	IC		bool	object_state			(u32 state_id, CObject *object);
	IC		u32		current_object_state_id	() const;
	IC		u32		current_state_state_id	() const;
			u32		object_state			() const;
			u32		weapon_state			(const CWeapon *weapon) const;
			CInventoryItem *best_weapon		() const;
	IC		bool	goal_reached			() const;
#ifdef DEBUG
			void	show_graph				();
#endif
};

#include "object_handler_inline.h"
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

template<typename _container_type>
struct CMapLocator {
	bool operator()(const typename _container_type::value_type &I, u32 id) const
	{
		return		(I.first < id);
	}
};

class CAI_Stalker;
class CWeapon;
class CMissile;
class CEatableItem;
class CObjectActionBase;

class CObjectHandlerGOAP : 
	public CActionPlanner<CAI_Stalker>,
	public CInventoryOwner

{
protected:
	typedef CActionPlanner<CAI_Stalker>	inherited;
	
	enum EWorldProperties {
		eWorldPropertyHidden		= u32(0),
		eWorldPropertyStrapping,
		eWorldPropertyStrapped,
		eWorldPropertyUnstrapping,
		eWorldPropertySwitch1,
		eWorldPropertySwitch2,
		eWorldPropertyAimed1,
		eWorldPropertyAimed2,
		eWorldPropertyAiming1,
		eWorldPropertyAiming2,
		eWorldPropertyEmpty1,
		eWorldPropertyEmpty2,
		eWorldPropertyReady1, // (magazine1 is full of ammo1) && !missfire1
		eWorldPropertyReady2, // (magazine2 is full of ammo2) && !missfire2
		eWorldPropertyFiring1,
		eWorldPropertyFiring2,
		eWorldPropertyAmmo1,
		eWorldPropertyAmmo2,
		eWorldPropertyIdle,
		eWorldPropertyIdleStrap,
		eWorldPropertyDropped,
		eWorldPropertyQueueWait,
		eWorldPropertyDummy			= u32(-1),
	};

	enum EWorldOperators {
		eWorldOperatorShow			= u32(0),
		eWorldOperatorHide,
		eWorldOperatorDrop,
		eWorldOperatorStrapping,
		eWorldOperatorStrapped,
		eWorldOperatorUnstrapping,
		eWorldOperatorIdle,
		eWorldOperatorAim1,
		eWorldOperatorAim2,
		eWorldOperatorReload1,
		eWorldOperatorReload2,
		eWorldOperatorFire1,
		eWorldOperatorFire2,
		eWorldOperatorSwitch1,
		eWorldOperatorSwitch2,
		eWorldOperatorQueueWait,
		eWorldOperatorDummy			= u32(-1),
	};

private:
	bool						m_aimed1;
	bool						m_aimed2;

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

protected:
			void			add_evaluators			(CWeapon		*weapon);
			void			add_operators			(CWeapon		*weapon);
			void			add_evaluators			(CMissile		*missile);
			void			add_operators			(CMissile		*missile);
			void			add_evaluators			(CEatableItem	*eatable_item);
			void			add_operators			(CEatableItem	*eatable_item);
			void			remove_evaluators		(CObject		*object);
			void			remove_operators		(CObject		*object);

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
	IC		u32				uid						(const u32 id1, const u32 id0) const;
	IC		bool			object_action			(u32 action_id, CObject *object);
	IC		u32				current_action_object_id() const;
	IC		u32				current_action_state_id	() const;
	IC		bool			goal_reached			() const;
			CInventoryItem	*best_weapon			() const;
			u32				weapon_state			(const CWeapon *weapon) const;
			void			add_item				(CInventoryItem *inventory_item);
			void			remove_item				(CInventoryItem *inventory_item);
			void			set_goal				(const MonsterSpace::EObjectAction object_action, CGameObject *game_object = 0);
	IC		void			add_condition			(CObjectActionBase *action, u16 id, EWorldProperties property, bool value);
	IC		void			add_effect				(CObjectActionBase *action, u16 id, EWorldProperties property, bool value);
};

#include "object_handler_goap_inline.h"
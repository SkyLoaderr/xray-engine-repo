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

class CObjectHandlerGOAP : 
	public CActionPlanner<CAI_Stalker>,
	public CInventoryOwner

{
public:
	enum EWorldProperties {
		eWorldPropertyItemID		= u32(0),
		eWorldPropertyHidden,
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
		eWorldPropertyQueueWait1,
		eWorldPropertyQueueWait2,
		eWorldPropertyAimingReady1,
		eWorldPropertyAimingReady2,
		
		eWorldPropertyThrowStarted,
		eWorldPropertyThrowIdle,
		eWorldPropertyThrow,
		eWorldPropertyThreaten,

		eWorldPropertyNoItems				= u32((u16(-1) << 16) | eWorldPropertyItemID),
		eWorldPropertyNoItemsIdle			= u32((u16(-1) << 16) | eWorldPropertyIdle),
		eWorldPropertyDummy					= u32(-1),
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
		eWorldOperatorQueueWait1,
		eWorldOperatorQueueWait2,
		eWorldOperatorAimingReady1,
		eWorldOperatorAimingReady2,

		eWorldOperatorThrowStart,
		eWorldOperatorThrowIdle,
		eWorldOperatorThrow,
		eWorldOperatorThreaten,

		eWorldOperatorNoItemsIdle	= u32((u16(-1) << 16) | eWorldOperatorIdle),
		eWorldOperatorDummy			= u32(-1),
	};

protected:
	typedef CActionPlanner<CAI_Stalker>				inherited;
	typedef CGraphEngine::_solver_value_type		_value_type;
	typedef CGraphEngine::_solver_condition_type	_condition_type;

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
	IC		EWorldProperties object_property		(MonsterSpace::EObjectAction object_action) const;
#ifdef LOG_ACTION
	virtual LPCSTR			action2string			(const _action_id_type &action_id);
	virtual LPCSTR			property2string			(const _condition_type &property_id);
#endif

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
	IC		_condition_type	uid						(const u32 id1, const u32 id0) const;
	IC		bool			object_action			(_condition_type action_id, CObject *object);
	IC		u32				current_action_object_id() const;
	IC		u32				current_action_state_id	() const;
	IC		u32				action_object_id		(_condition_type action_id) const;
	IC		u32				action_state_id			(_condition_type action_id) const;
	IC		bool			goal_reached			() const;
			CInventoryItem	*best_weapon			() const;
			u32				weapon_state			(const CWeapon *weapon) const;
			void			add_item				(CInventoryItem *inventory_item);
			void			remove_item				(CInventoryItem *inventory_item);
			void			set_goal				(MonsterSpace::EObjectAction object_action, CGameObject *game_object = 0);
	IC		void			add_condition			(CActionBase<CAI_Stalker> *action, u16 id, EWorldProperties property, _value_type value);
	IC		void			add_effect				(CActionBase<CAI_Stalker> *action, u16 id, EWorldProperties property, _value_type value);
};

#include "object_handler_goap_inline.h"
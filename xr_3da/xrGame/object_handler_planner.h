////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_planner.h
//	Created 	: 11.03.2004
//  Modified 	: 01.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler action planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner.h"

class CAI_Stalker;
class CInventoryItem;
class CWeapon;
class CMissile;
class CFoodItem;

namespace ObjectHandlerSpace {
	enum EWorldProperties;
};

namespace MonsterSpace {
	enum EObjectAction;
};

class CObjectHandlerPlanner : public CActionPlanner<CAI_Stalker,true> {
public:
	typedef CActionPlanner<CAI_Stalker,true>			inherited;
	typedef GraphEngineSpace::_solver_value_type		_value_type;
	typedef GraphEngineSpace::_solver_condition_type	_condition_type;

protected:
			void			add_evaluators			(CWeapon		*weapon);
			void			add_operators			(CWeapon		*weapon);
			void			add_evaluators			(CMissile		*missile);
			void			add_operators			(CMissile		*missile);
			void			add_evaluators			(CFoodItem		*food_item);
			void			add_operators			(CFoodItem		*food_item);
			void			remove_evaluators		(CObject		*object);
			void			remove_operators		(CObject		*object);
	IC		ObjectHandlerSpace::EWorldProperties object_property		(MonsterSpace::EObjectAction object_action) const;
#ifdef LOG_ACTION
	virtual LPCSTR			action2string			(const _action_id_type &action_id);
	virtual LPCSTR			property2string			(const _condition_type &property_id);
#endif

public:
	IC		_condition_type	uid						(const u32 id1, const u32 id0) const;
	IC		bool			object_action			(_condition_type action_id, CObject *object);
	IC		u32				current_action_object_id() const;
	IC		u32				current_action_state_id	() const;
	IC		u32				action_object_id		(_condition_type action_id) const;
	IC		u32				action_state_id			(_condition_type action_id) const;
	IC		void			add_condition			(CActionBase<CAI_Stalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value);
	IC		void			add_effect				(CActionBase<CAI_Stalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value);
	IC		CAI_Stalker		&object					() const;

public:
	virtual	void			setup					(CAI_Stalker *object);
	virtual	void			update					();
			void			add_item				(CInventoryItem *inventory_item);
			void			remove_item				(CInventoryItem *inventory_item);
			void			set_goal				(MonsterSpace::EObjectAction object_action, CGameObject *game_object = 0, u32 queue_size = 0, u32 queue_interval = 300);
};

#include "object_handler_planner_inline.h"
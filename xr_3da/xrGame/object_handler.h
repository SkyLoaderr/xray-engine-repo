////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.h
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventoryowner.h"
#include "graph_engine_space.h"

template<typename _container_type>
struct CMapLocator {
	bool operator()(const typename _container_type::value_type &I, u32 id) const
	{
		return		(I.first < id);
	}
};

namespace MonsterSpace {
	enum EObjectAction;
}

class CAI_Stalker;
class CWeapon;
class CMissile;
class CFoodItem;
class CObjectHandlerPlanner;

class CObjectHandler : public CInventoryOwner {
protected:
	typedef CInventoryOwner								inherited;
	typedef GraphEngineSpace::_solver_value_type		_value_type;
	typedef GraphEngineSpace::_solver_condition_type	_condition_type;

private:
	int								m_r_hand;
	int								m_l_finger1;
	int								m_r_finger2;


protected:
	bool							m_hammer_is_clutched;
	bool							m_infinite_ammo;
	CObjectHandlerPlanner			*m_planner;

public:
									CObjectHandler		();
	virtual							~CObjectHandler		();
	virtual	void					Load				(LPCSTR section);
	virtual	void					reinit				(CAI_Stalker *object);
	virtual	void					reload				(LPCSTR section);
	virtual BOOL					net_Spawn			(LPVOID DC);
	virtual	void					update				();
	virtual void					OnItemTake			(CInventoryItem *inventory_item);
	virtual void					OnItemDrop			(CInventoryItem *inventory_item);
	virtual void					OnItemDropUpdate	();
			CInventoryItem			*best_weapon		() const;
			void					set_goal			(MonsterSpace::EObjectAction object_action, CGameObject *game_object = 0, u32 queue_size = 0, u32 queue_interval = 300);
			bool					goal_reached		();
	IC		bool					hammer_is_clutched	() const;
	IC		CObjectHandlerPlanner	&planner			() const;
			void					weapon_bones		(int &b0, int &b1, int &b2) const;
};

#include "object_handler_inline.h"
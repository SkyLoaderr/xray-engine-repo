////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap.cpp
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "weapon.h"
#include "missile.h"
#include "enemy_manager.h"
#include "custommonster.h"
#include "weaponmagazined.h"
#include "ef_storage.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "fooditem.h"

#include "object_handler_goap.h"
#include "object_action_base.h"
#include "object_property_evaluator_base.h"
//#include "object_state_base.h"
//#include "object_state_show.h"
//#include "object_state_hide.h"
//#include "object_state_fire_primary.h"
//#include "object_state_reload.h"
//#include "object_state_empty.h"
//#include "object_state_switch.h"
//#include "object_state_idle.h"

CObjectHandlerGOAP::CObjectHandlerGOAP	()
{
	init						();
}

CObjectHandlerGOAP::~CObjectHandlerGOAP	()
{
}

void CObjectHandlerGOAP::init	()
{
}

void CObjectHandlerGOAP::Load			(LPCSTR section)
{
	inherited::Load				(section);
	CInventoryOwner::Load		(section);
	m_dwFireRandomMin  			= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax  			= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin  			= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  			= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance 			= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor   			= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance 			= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor			= pSettings->r_float(section,"MaxMissFactor");
	m_bCanFire					= true;
	m_bHammerIsClutched			= false;
	m_dwNoFireTime				= 0;
	m_dwStartFireTime			= 0;
}

void CObjectHandlerGOAP::reinit			(CAI_Stalker *object)
{
	inherited::reinit			(object,true);
	CInventoryOwner::reinit		();
}

void CObjectHandlerGOAP::reload			(LPCSTR section)
{
	inherited::reload			(section);
	CInventoryOwner::reload		(section);
}

void CObjectHandlerGOAP::update(u32 time_delta)
{
	inherited::update			(time_delta);
}

void CObjectHandlerGOAP::OnItemTake		(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake	(inventory_item);
	Msg							("Adding item %s (%d)",*inventory_item->cName(),inventory_item->ID());
	add_item				(inventory_item);
}

void CObjectHandlerGOAP::OnItemDrop		(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop	(inventory_item);
	Msg							("Removing item %s (%d)",*inventory_item->cName(),inventory_item->ID());
	remove_item					(inventory_item);
}

void CObjectHandlerGOAP::OnItemDropUpdate	()
{
	CInventoryOwner::OnItemDropUpdate	();
}

CInventoryItem *CObjectHandlerGOAP::best_weapon() const
{
	CInventoryItem	*best_weapon = 0;
	u32				best_weapon_type = 0;
	ai().ef_storage().m_tpCurrentMember = m_object;
	TIItemSet::const_iterator	I = inventory().m_all.begin();
	TIItemSet::const_iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if ((*I)->getDestroy())
			continue;
		CWeapon		*weapon = dynamic_cast<CWeapon*>(*I);
		if (weapon && (weapon->GetAmmoCurrent() > weapon->GetAmmoMagSize()/10)) {
			ai().ef_storage().m_tpGameObject	= weapon;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
			continue;
		}
		CMissile	*missile = dynamic_cast<CMissile*>(*I);
		if (missile) {
			ai().ef_storage().m_tpGameObject	= missile;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
		}
	}
	return			(best_weapon);
}

u32 CObjectHandlerGOAP::weapon_state(const CWeapon *weapon) const
{
	if (!weapon)
		return		(u32(-1));

	switch (weapon->STATE) {
		case CWeapon::eIdle		:  {
			const CWeaponMagazined	*weapon_magazined = dynamic_cast<const CWeaponMagazined*>(weapon);
			if (!weapon_magazined || (weapon_magazined->GetAmmoElapsed() && !weapon_magazined->IsMisfire()))
				return(CWeapon::eIdle);
			if (!weapon_magazined->GetAmmoElapsed() && (current_action_state_id() != eObjectActionReload1))
				return(CWeapon::eMagEmpty);
			if (weapon_magazined->IsMisfire() && (current_action_state_id() != eObjectActionReload1))
				return(CWeapon::eMisfire);
			
			switch (current_action_state_id()) {
				case eObjectActionIdle		: return(CWeapon::eIdle		);
				case eObjectActionFire1		: return(CWeapon::eFire		);
				case eObjectActionFire2		: return(CWeapon::eFire2	);
				case eObjectActionReload1	: return(CWeapon::eReload	);
				case eObjectActionShow		: return(CWeapon::eShowing	);
				case eObjectActionHide		: return(CWeapon::eHiding	);
			}
		}
		default : 
			return(weapon->STATE);
	}
}

u32 CObjectHandlerGOAP::object_action() const
{
	if (!inventory().ActiveItem() || !inventory().ActiveItem()->H_Parent() || inventory().ActiveItem()->getDestroy())
		return			(eObjectActionNoItems);

	CWeapon				*weapon = dynamic_cast<CWeapon*>(inventory().ActiveItem());
	if (weapon) {
		if (weapon->ID() != current_action_object_id()) {
			switch (weapon_state(weapon)) {
				case CWeapon::eIdle		: return(uid(eObjectActionIdle,weapon->ID()));
				case CWeapon::eFire		: return(uid(eObjectActionFire1,weapon->ID()));
				case CWeapon::eFire2	: return(uid(eObjectActionFire2,weapon->ID()));
				case CWeapon::eReload	: return(uid(eObjectActionReload1,weapon->ID()));
				case CWeapon::eShowing	: return(uid(eObjectActionShow,weapon->ID()));
				case CWeapon::eHiding	: return(uid(eObjectActionHide,weapon->ID()));
				case CWeapon::eMisfire	: return(uid(eObjectActionMisfire1,weapon->ID()));
				case CWeapon::eMagEmpty : return(uid(eObjectActionEmpty1,weapon->ID()));
				case CWeapon::eHidden	: return(eObjectActionNoItems);
				default			: NODEFAULT;
			}
		}
		switch (weapon_state(weapon)) {
			case CWeapon::eIdle : {
				switch (current_action_state_id()) {
					case eObjectActionIdle :
					case eObjectActionAim1 :
					case eObjectActionAim2 :
					case eObjectActionSwitch1 :
					case eObjectActionSwitch2 : 
						return	(current_action_id());
					default :
						if (!weapon->IsWorking())
							return	(uid(eObjectActionIdle,weapon->ID()));
						else
							return	(current_action_id());
				}
			}
			case CWeapon::eFire : {
				switch (current_action_state_id()) {
					case eObjectActionFire1 :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionFire1,weapon->ID()));
				}
			}
			case CWeapon::eFire2 : {
				switch (current_action_state_id()) {
					case eObjectActionFire2 :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionFire2,weapon->ID()));
				}
			}
			case CWeapon::eReload : {
				switch (current_action_state_id()) {
					case eObjectActionReload1 :
					case eObjectActionReload2 :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionReload1,weapon->ID()));
				}
			}
			case CWeapon::eShowing : {
				switch (current_action_state_id()) {
					case eObjectActionShow :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionShow,weapon->ID()));
				}
			}
			case CWeapon::eHiding : {
				switch (current_action_state_id()) {
					case eObjectActionHide :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionHide,weapon->ID()));
				}
			}
			case CWeapon::eMisfire : {
				switch (current_action_state_id()) {
					case eObjectActionMisfire1 :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionMisfire1,weapon->ID()));
				}
			}
			case CWeapon::eMagEmpty : {
				switch (current_action_state_id()) {
					case eObjectActionEmpty1 :
						return	(current_action_id());
					default : 
						return	(uid(eObjectActionEmpty1,weapon->ID()));
				}
			}
			case CWeapon::eHidden :
				return	(current_action_id());
			default : NODEFAULT;
		}
	}

	CMissile		*missile = dynamic_cast<CMissile*>(inventory().ActiveItem());
	if (missile) {
		switch (missile->State()) {
			case MS_HIDDEN	 : return(eObjectActionNoItems);
			case MS_SHOWING	 : return(uid(eObjectActionShow,missile->ID()));
			case MS_HIDING	 : return(uid(eObjectActionHide,missile->ID()));
			case MS_IDLE	 : return(uid(eObjectActionIdle,missile->ID()));
			case MS_EMPTY	 : return(uid(eObjectActionEmpty1,missile->ID()));
			case MS_THREATEN : return(uid(eObjectActionFire2,missile->ID()));
			case MS_READY	 : return(uid(eObjectActionSwitch1,missile->ID()));
			case MS_THROW	 : return(uid(eObjectActionFire1,missile->ID()));
			case MS_END		 : return(uid(eObjectActionFire1,missile->ID()));
			case MS_PLAYING	 : return(uid(eObjectActionIdle,missile->ID()));
			default			 : NODEFAULT;
		}
	}

	CFoodItem		*food_item = dynamic_cast<CFoodItem*>(inventory().ActiveItem());
	if (food_item) {
		switch (food_item->STATE) {
			case FOOD_HIDDEN : return(eObjectActionNoItems);
			case FOOD_SHOWING: return(uid(eObjectActionShow,food_item->ID()));
			case FOOD_HIDING : return(uid(eObjectActionHide,food_item->ID()));
			case FOOD_IDLE	 : return(uid(eObjectActionIdle,food_item->ID()));
			case FOOD_PREPARE: return(uid(eObjectActionSwitch1,food_item->ID()));
			case FOOD_EATING : return(uid(eObjectActionFire1,food_item->ID()));
			default			 : NODEFAULT;
		}
	}
	
	NODEFAULT;

#ifdef DEBUG
	return			(u32(eObjectActionDummy));
#endif
}

void CObjectHandlerGOAP::add_item			(CInventoryItem *inventory_item)
{
//	u32					id = inventory_item->ID();
//	add_state			(xr_new<CObjectStateBase>(inventory_item,CWeapon::eHiding),	uid(eObjectActionDrop,id),		0);
	
	CWeapon				*weapon		= dynamic_cast<CWeapon*>		(inventory_item);
	CMissile			*missile	= dynamic_cast<CMissile*>		(inventory_item);
	CEatableItem		*eatable	= dynamic_cast<CEatableItem*>	(inventory_item);

	if (weapon) {
//		add_state		(xr_new<CObjectStateBase>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionIdle,id),		0);
//		add_state		(xr_new<CObjectStateShow>(inventory_item,CWeapon::eShowing),	uid(eObjectActionShow,id),		0);
//		add_state		(xr_new<CObjectStateHide>(inventory_item,CWeapon::eHiding),		uid(eObjectActionHide,id),		0);
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionStrap,id),		0);
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionAim1,id),		0);
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionAim2,id),		0);
//		add_state		(xr_new<CObjectStateReload>(inventory_item,CWeapon::eReload),	uid(eObjectActionReload1,id),	0);
//		add_state		(xr_new<CObjectStateReload>(inventory_item,CWeapon::eReload),	uid(eObjectActionReload2,id),	0);
//		add_state		(xr_new<CObjectStateFirePrimary>(inventory_item,CWeapon::eFire),uid(eObjectActionFire1,id),		0);
//		add_state		(xr_new<CObjectStateBase>(inventory_item,CWeapon::eFire2),		uid(eObjectActionFire2,id),		0);
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionSwitch1,id),	0);
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionSwitch2,id),	0);
//		add_state		(xr_new<CObjectStateEmpty>(inventory_item,CWeapon::eFire),		uid(eObjectActionMisfire1,id),	0);
//		add_state		(xr_new<CObjectStateEmpty>(inventory_item,CWeapon::eFire),		uid(eObjectActionEmpty1,id),	0);
//
//		add_transition	(uid(eObjectActionShow,id),			uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionHide,id),		1,		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionDrop,id),		1);
//		add_transition	(uid(eObjectActionStrap,id),		uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionAim1,id),			uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionAim2,id),			uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionReload1,id),		uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionReload2,id),		uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionFire1,id),		uid(eObjectActionIdle,id),		1);
//		add_transition	(uid(eObjectActionFire2,id),		uid(eObjectActionIdle,id),		1);
//		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionIdle,id),		1);
//		add_transition	(uid(eObjectActionSwitch2,id),		uid(eObjectActionIdle,id),		1);
//		add_transition	(uid(eObjectActionReload1,id),		uid(eObjectActionAim1,id),		1,		1);
//		add_transition	(uid(eObjectActionReload2,id),		uid(eObjectActionAim1,id),		1,		1);
//		add_transition	(uid(eObjectActionReload1,id),		uid(eObjectActionAim2,id),		1,		1);
//		add_transition	(uid(eObjectActionReload2,id),		uid(eObjectActionAim2,id),		1,		1);
//		add_transition	(uid(eObjectActionAim1,id),			uid(eObjectActionAim2,id),		1,		1);
//		add_transition	(uid(eObjectActionFire1,id),		uid(eObjectActionAim1,id),		1,		1);
//		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionAim1,id),		1);
//		add_transition	(uid(eObjectActionAim1,id),			uid(eObjectActionSwitch2,id),	1);
//		add_transition	(uid(eObjectActionFire2,id),		uid(eObjectActionAim2,id),		1,		1);
//		add_transition	(uid(eObjectActionSwitch2,id),		uid(eObjectActionAim2,id),		1);
//		add_transition	(uid(eObjectActionAim2,id),			uid(eObjectActionSwitch1,id),	1);
//		add_transition	(uid(eObjectActionMisfire1,id),		uid(eObjectActionReload1,id),	1);
//		add_transition	(uid(eObjectActionMisfire1,id),		uid(eObjectActionHide,id),		10);
//		add_transition	(uid(eObjectActionEmpty1,id),		uid(eObjectActionReload1,id),	1);
//		add_transition	(uid(eObjectActionEmpty1,id),		uid(eObjectActionHide,id),		10);
//
//		state(uid(eObjectActionIdle,id)).set_inertia_time(0);
//		state(uid(eObjectActionAim1,id)).set_inertia_time(1000);
//		state(uid(eObjectActionAim2,id)).set_inertia_time(1000);
	}
	else if (missile) {
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,MS_IDLE,true),			uid(eObjectActionIdle,id),		0);
////		add_state		(xr_new<CObjectStateShow>(inventory_item,MS_SHOWING),			uid(eObjectActionShow,id),		0);
//		add_state		(xr_new<CObjectStateShow>(inventory_item,MS_IDLE),				uid(eObjectActionShow,id),		0);
//		add_state		(xr_new<CObjectStateHide>(inventory_item,MS_HIDING),			uid(eObjectActionHide,id),		0);
//		add_state		(xr_new<CObjectStateFirePrimary>(inventory_item,MS_THREATEN),	uid(eObjectActionFire2,id),		0);
//		add_state		(xr_new<CObjectStateSwitch>(inventory_item,MS_READY,true),		uid(eObjectActionSwitch1,id),	0);
//		add_state		(xr_new<CObjectStateBase>(inventory_item,MS_END),				uid(eObjectActionFire1,id),		0);
//
//		add_transition	(uid(eObjectActionShow,id),			uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionHide,id),		1,		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionDrop,id),		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionFire2,id),		1);
//		add_transition	(uid(eObjectActionFire2,id),		uid(eObjectActionSwitch1,id),	1);
//		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionFire1,id),		1);
//		add_transition	(uid(eObjectActionFire1,id),		u32(eObjectActionNoItems),		1);
//
//		state(uid(eObjectActionSwitch1,id)).set_inertia_time(100);
//		state(uid(eObjectActionFire1,id)).set_inertia_time(1000);
	}
	else if (eatable) {
//		add_state		(xr_new<CObjectStateIdle>(inventory_item,FOOD_IDLE,true),		uid(eObjectActionIdle,id),		0);
//		add_state		(xr_new<CObjectStateShow>(inventory_item,FOOD_IDLE),			uid(eObjectActionShow,id),		0);
//		add_state		(xr_new<CObjectStateHide>(inventory_item,FOOD_HIDING),			uid(eObjectActionHide,id),		0);
//		add_state		(xr_new<CObjectStateSwitch>(inventory_item,FOOD_PREPARE,true),	uid(eObjectActionSwitch1,id),	0);
//		add_state		(xr_new<CObjectStateBase>(inventory_item,FOOD_EATING),			uid(eObjectActionFire1,id),		0);
//
//		add_transition	(uid(eObjectActionShow,id),			uid(eObjectActionIdle,id),		1,		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionHide,id),		1,		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionDrop,id),		1);
//		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionSwitch1,id),	1,		1);
//		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionFire1,id),		1);
//		add_transition	(uid(eObjectActionFire1,id),		u32(eObjectActionNoItems),		1);
	}

	// нож, приборы

	//
//	if (graph().vertex(uid(eObjectActionHide,inventory_item->ID())))
//		add_transition		(uid(eObjectActionHide,inventory_item->ID()),u32(eObjectActionNoItems),1.f);
//	if (graph().vertex(uid(eObjectActionShow,inventory_item->ID())))
//		add_transition		(u32(eObjectActionNoItems),uid(eObjectActionShow,inventory_item->ID()),1.f);
}

void CObjectHandlerGOAP::remove_item		(CInventoryItem *inventory_item)
{
//	u32				id = inventory_item->ID();
//	remove_state	(uid(eObjectActionDrop,			id));
//
//	CWeapon			*weapon = dynamic_cast<CWeapon*>(inventory_item);
//	CMissile		*missile = dynamic_cast<CMissile*>(inventory_item);
//
//	if (weapon) {
//		remove_state	(uid(eObjectActionIdle,id));
//		remove_state	(uid(eObjectActionShow,id));
//		remove_state	(uid(eObjectActionHide,id));
//		remove_state	(uid(eObjectActionStrap,id));
//		remove_state	(uid(eObjectActionAim1,id));
//		remove_state	(uid(eObjectActionAim2,id));
//		remove_state	(uid(eObjectActionReload1,id));
//		remove_state	(uid(eObjectActionReload2,id));
//		remove_state	(uid(eObjectActionFire1,id));
//		remove_state	(uid(eObjectActionFire2,id));
//		remove_state	(uid(eObjectActionSwitch1,id));
//		remove_state	(uid(eObjectActionSwitch2,id));
//		return;
//	}
//
//	if (missile) {
//		remove_state	(uid(eObjectActionIdle,id));
//		remove_state	(uid(eObjectActionShow,id));
//		remove_state	(uid(eObjectActionHide,id));
//		remove_state	(uid(eObjectActionFire1,id));
//		remove_state	(uid(eObjectActionSwitch1,id));
//		remove_state	(uid(eObjectActionFire2,id));
//		return;
//	}
}

void CObjectHandlerGOAP::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object)
{
//	if (object_action == MonsterSpace::eObjectActionActivate)
//		object_action					= MonsterSpace::eObjectActionIdle;
//
//	if (object_action == MonsterSpace::eObjectActionDeactivate)
//		object_action					= MonsterSpace::eObjectActionNoItems;
//
//	if (object_action == MonsterSpace::eObjectActionNoItems) {
//		inherited::set_dest_state		(object_action);
//		return;
//	}
//
//	if (game_object)
//		inherited::set_dest_state		(uid(object_action, game_object->ID()));
//	else
//		if (inventory().ActiveItem())
//			inherited::set_dest_state	(uid(object_action, inventory().ActiveItem()->ID()));
//		else
//			inherited::set_dest_state	(u32(MonsterSpace::eObjectActionNoItems));
}
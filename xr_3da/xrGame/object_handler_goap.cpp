////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap.cpp
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_handler_goap.h"
//#include "object_state_base.h"
//#include "object_state_show.h"
//#include "object_state_hide.h"
//#include "object_state_fire_primary.h"
//#include "object_state_reload.h"
//#include "object_state_empty.h"
//#include "object_state_switch.h"
//#include "object_state_idle.h"

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

CObjectHandlerGOAP::CObjectHandlerGOAP	()
{
	init						();
}

CObjectHandlerGOAP::~CObjectHandlerGOAP	()
{
}

void CObjectHandlerGOAP::init			()
{
}

void CObjectHandlerGOAP::Load			(LPCSTR section)
{
//	inherited::Load					(section);
	CInventoryOwner::Load			(section);
//	m_dwFireRandomMin  				= pSettings->r_s32(section,"FireRandomMin");
//	m_dwFireRandomMax  				= pSettings->r_s32(section,"FireRandomMax");
//	m_dwNoFireTimeMin  				= pSettings->r_s32(section,"NoFireTimeMin");
//	m_dwNoFireTimeMax  				= pSettings->r_s32(section,"NoFireTimeMax");
//	m_fMinMissDistance 				= pSettings->r_float(section,"MinMissDistance");
//	m_fMinMissFactor   				= pSettings->r_float(section,"MinMissFactor");
//	m_fMaxMissDistance 				= pSettings->r_float(section,"MaxMissDistance");
//	m_fMaxMissFactor				= pSettings->r_float(section,"MaxMissFactor");
//	m_bCanFire						= true;
//	m_bHammerIsClutched				= false;
//	m_dwNoFireTime					= 0;
//	m_dwStartFireTime				= 0;
}

void CObjectHandlerGOAP::reinit			(CAI_Stalker *object)
{
//	inherited::reinit				(object,true);
	CInventoryOwner::reinit			();
}

void CObjectHandlerGOAP::reload			(LPCSTR section)
{
//	inherited::reload				(section);
	CInventoryOwner::reload			(section);
}

void CObjectHandlerGOAP::update(u32 time_delta)
{
//	inherited::update	(time_delta);
}

void CObjectHandlerGOAP::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake	(inventory_item);
	Msg						("Adding item %s (%d)",*inventory_item->cName(),inventory_item->ID());
}

void CObjectHandlerGOAP::OnItemDrop			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop	(inventory_item);
	Msg						("Removing item %s (%d)",*inventory_item->cName(),inventory_item->ID());
}

void CObjectHandlerGOAP::OnItemDropUpdate	()
{
	CInventoryOwner::OnItemDropUpdate		();
}

CInventoryItem *CObjectHandlerGOAP::best_weapon() const
{
//	CInventoryItem	*best_weapon = 0;
//	u32				best_weapon_type = 0;
//	ai().ef_storage().m_tpCurrentMember = m_object;
//	TIItemSet::const_iterator	I = inventory().m_all.begin();
//	TIItemSet::const_iterator	E = inventory().m_all.end();
//	for ( ; I != E; ++I) {
//		if ((*I)->getDestroy())
//			continue;
//		CWeapon		*weapon = dynamic_cast<CWeapon*>(*I);
//		if (weapon && (weapon->GetAmmoCurrent() > weapon->GetAmmoMagSize()/10)) {
//			ai().ef_storage().m_tpGameObject	= weapon;
//			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
//			if (current_weapon_type > best_weapon_type) {
//				best_weapon_type = current_weapon_type;
//				best_weapon		 = *I;
//			}
//			continue;
//		}
//		CMissile	*missile = dynamic_cast<CMissile*>(*I);
//		if (missile) {
//			ai().ef_storage().m_tpGameObject	= missile;
//			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
//			if (current_weapon_type > best_weapon_type) {
//				best_weapon_type = current_weapon_type;
//				best_weapon		 = *I;
//			}
//		}
//	}
//	return			(best_weapon);
	return			(0);
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
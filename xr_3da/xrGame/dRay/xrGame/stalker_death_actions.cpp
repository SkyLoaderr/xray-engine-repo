////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_death_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker death action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_death_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"
#include "script_game_object.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "stalker_movement_manager.h"
#include "inventory.h"
#include "weapon.h"
#include "clsid_game.h"
#include "xr_level_controller.h"

using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

CStalkerActionDead::CStalkerActionDead	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDead::initialize		()
{
	inherited::initialize	();
	if (object().inventory().TotalWeight() <= 0)
		return;
	
	CWeapon					*tpWeapon = smart_cast<CWeapon*>(object().inventory().ActiveItem());
	if (!(!tpWeapon || !tpWeapon->GetAmmoElapsed() || !object().hammer_is_clutched() || (Device.dwTimeGlobal - object().GetLevelDeathTime() > 500))) {
		object().inventory().Action(kWPN_FIRE,	CMD_START);
		xr_vector<CInventorySlot>::iterator I = object().inventory().m_slots.begin(), B = I;
		xr_vector<CInventorySlot>::iterator E = object().inventory().m_slots.end();
		for ( ; I != E; ++I)
			if ((I - B) != (int)object().inventory().GetActiveSlot())
				if ((*I).m_pIItem && ((*I).m_pIItem->object().CLS_ID != CLSID_IITEM_BOLT))
					object().inventory().Ruck((*I).m_pIItem);
	}
}

void CStalkerActionDead::execute		()
{
	inherited::execute		();
	object().movement().enable_movement(false);

	CWeapon					*tpWeapon = smart_cast<CWeapon*>(object().inventory().ActiveItem());
	if (!tpWeapon || !tpWeapon->GetAmmoElapsed() || !object().hammer_is_clutched() || (Device.dwTimeGlobal - object().GetLevelDeathTime() > 500)) {
		xr_vector<CInventorySlot>::iterator I = object().inventory().m_slots.begin(), B = I;
		xr_vector<CInventorySlot>::iterator E = object().inventory().m_slots.end();
		for ( ; I != E; ++I)
			if ((I - B) == (int)object().inventory().GetActiveSlot()) {
				if ((*I).m_pIItem && (*I).m_pIItem->object().CLS_ID != CLSID_IITEM_BOLT) {
					(*I).m_pIItem->Drop();
//					Msg("----CAI_Stalker called Drop for inventoryItem[%d] time is [%f]",(*I).m_pIItem->object().ID(),Device.fTimeGlobal);
				}
			}
			else
				if((*I).m_pIItem)
					if ((*I).m_pIItem->object().CLS_ID != CLSID_IITEM_BOLT)
						object().inventory().Ruck((*I).m_pIItem);

		set_property		(eWorldPropertyDead,true);
	}
}


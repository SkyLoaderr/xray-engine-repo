////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../actor.h"
#include "../../bolt.h"
#include "../../ai_script_actions.h"
#include "../../ef_storage.h"
#include "../../inventory.h"

bool CAI_Stalker::useful		(const CGameObject *object) const
{
	if (!CItemManager::useful(object))
		return			(false);

	const CInventoryItem *inventory_item = dynamic_cast<const CInventoryItem*>(object);
	if (!inventory_item || !inventory_item->Useful())
		return			(false);

	const CBolt			*bolt = dynamic_cast<const CBolt*>(object);
	if (bolt)
		return			(false);

	CInventory			*inventory_non_const = const_cast<CInventory*>(&inventory());
	CInventoryItem		*inventory_item_non_const = const_cast<CInventoryItem*>(inventory_item);
	if (!inventory_non_const->CanTakeItem(inventory_item_non_const))
		return			(false);

	return				(true);
}

float CAI_Stalker::evaluate		(const CGameObject *object) const
{
	float				distance = Position().distance_to_sqr(object->Position());
	distance			= !fis_zero(distance) ? distance : EPS_L;
	return				(distance);
}

bool CAI_Stalker::bfIfHuman(const CEntity *tpEntity)
{
	if (!tpEntity)
		tpEntity = enemy();
	if (!tpEntity)
		return(false);
	switch (tpEntity->SUB_CLS_ID) {
		case CLSID_OBJECT_ACTOR :
		case CLSID_AI_SCIENTIST :
		case CLSID_AI_SOLDIER :
		case CLSID_AI_STALKER_MILITARY :
		case CLSID_AI_STALKER : 
			return(true);
		default : return(false);
	}
}

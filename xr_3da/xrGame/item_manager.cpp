////////////////////////////////////////////////////////////////////////////
//	Module 		: item_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Item manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "item_manager.h"
#include "inventory_item.h"

bool CItemManager::useful			(const CGameObject  *object) const
{
	if (!inherited::useful(object))
		return				(false);

	if (object->getDestroy())
		return				(false);

	// we do not want to keep in memory attached objects
	if (object->H_Parent())
		return				(false);

	const CInventoryItem	*inventory_item = dynamic_cast<const CInventoryItem*>(object);
	if (inventory_item && !inventory_item->useful_for_NPC())
		return				(false);

	if (!ai().get_level_graph() || !ai().level_graph().valid_vertex_id(object->level_vertex_id()))
		return				(false);

	return					(true);
}

float CItemManager::evaluate		(const CGameObject *object) const
{
	const CInventoryItem	*inventory_item = dynamic_cast<const CInventoryItem*>(object);
	VERIFY					(inventory_item);
	VERIFY					(inventory_item->useful_for_NPC());
	return					(1000000.f - (float)inventory_item->Cost());
}


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

	return					(true);
}

float CItemManager::evaluate		(const CGameObject *object) const
{
	const CInventoryItem	*inventory_item = dynamic_cast<const CInventoryItem*>(object);
	if (!inventory_item)
		return				(flt_max);
	return					(1000000.f - (float)inventory_item->Cost());
}


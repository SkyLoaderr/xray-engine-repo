////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../bolt.h"
#include "../../inventory.h"
#include "../../character_info.h"
#include "../../relation_registry.h"
#include "../../memory_manager.h"
#include "../../item_manager.h"

bool CAI_Stalker::useful		(const CGameObject *object) const
{
	if (!memory().item().useful(object))
		return			(false);

	const CInventoryItem *inventory_item = smart_cast<const CInventoryItem*>(object);
	if (!inventory_item || !inventory_item->Useful())
		return			(false);

	const CBolt			*bolt = smart_cast<const CBolt*>(object);
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

ALife::ERelationType  CAI_Stalker::tfGetRelationType	(const CEntityAlive *tpEntityAlive) const
{
	const CInventoryOwner* pOtherIO = smart_cast<const CInventoryOwner*>(tpEntityAlive);
	
	ALife::ERelationType relation = ALife::eRelationTypeDummy;
		
	if(pOtherIO)
		relation = RELATION_REGISTRY().GetRelationType(static_cast<const CInventoryOwner*>(this), pOtherIO);
	
	if(ALife::eRelationTypeDummy != relation)
		return relation;
	else
		return inherited::tfGetRelationType(tpEntityAlive);
}

void CAI_Stalker::adjust_speed_to_animation	(const EMovementDirection &movement_direction)
{
	if (body_state() == eBodyStateStand) {
		if (movement_direction != eMovementDirectionBack) {
			if (movement_type() == eMovementTypeWalk)
				set_desirable_speed(m_fCurSpeed = m_fWalkFactor);
			else
				if (movement_type() == eMovementTypeRun)
					set_desirable_speed(m_fCurSpeed = m_fRunFactor);
		}
		else {
			if (movement_type() == eMovementTypeWalk)
				set_desirable_speed(m_fCurSpeed = m_fWalkBackFactor);
			else
				if (movement_type() == eMovementTypeRun)
					set_desirable_speed(m_fCurSpeed = m_fRunBackFactor);
		}
	}
}


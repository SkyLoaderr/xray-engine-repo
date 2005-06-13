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
#include "../../stalker_movement_manager.h"
#include "../../explosive.h"
#include "../../agent_manager.h"
#include "../../agent_member_manager.h"
#include "../../agent_explosive_manager.h"
#include "../../member_order.h"
#include "../../level.h"
#include "../../sound_player.h"
#include "ai_stalker_space.h"
#include "../../danger_manager.h"

const u32 TOLLS_INTERVAL	= 2000;
const u32 GRENADE_INTERVAL	= 0*1000;
const float FRIENDLY_GRENADE_ALARM_DIST	= 5.f;

bool CAI_Stalker::useful		(const CItemManager *manager, const CGameObject *object) const
{
	const CExplosive	*explosive = smart_cast<const CExplosive*>(object);
	if (explosive && (explosive->CurrentParentID() != 0xffff)) {
		agent_manager().explosive().register_explosive(explosive,object);
		CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(Level().Objects.net_Find(explosive->CurrentParentID()));
		if (entity_alive)
			memory().danger().add(CDangerObject(entity_alive,object->Position(),Device.dwTimeGlobal,CDangerObject::eDangerTypeGrenade,CDangerObject::eDangerPerceiveTypeVisual,object));
	}

	if (!memory().item().useful(object))
		return			(false);

	const CInventoryItem *inventory_item = smart_cast<const CInventoryItem*>(object);
	if (!inventory_item || !inventory_item->useful_for_NPC())
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

float CAI_Stalker::evaluate		(const CItemManager *manager, const CGameObject *object) const
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
	if (movement().body_state() == eBodyStateStand) {
		if (movement_direction != eMovementDirectionBack) {
			if (movement().movement_type() == eMovementTypeWalk)
				movement().set_desirable_speed(m_fCurSpeed = movement().walk_factor());
			else
				if (movement().movement_type() == eMovementTypeRun)
					movement().set_desirable_speed(m_fCurSpeed = movement().run_factor());
		}
		else {
			if (movement().movement_type() == eMovementTypeWalk)
				movement().set_desirable_speed(m_fCurSpeed = movement().walk_back_factor());
			else
				if (movement().movement_type() == eMovementTypeRun)
					movement().set_desirable_speed(m_fCurSpeed = movement().run_back_factor());
		}
	}
}

void CAI_Stalker::react_on_grenades		()
{
	CMemberOrder::CGrenadeReaction	&reaction = agent_manager().member().member(this).grenade_reaction();
	if (!reaction.m_processing)
		return;

	if (Device.dwTimeGlobal < reaction.m_time + GRENADE_INTERVAL)
		return;

//	u32							interval = AFTER_GRENADE_DESTROYED_INTERVAL;
	const CMissile				*missile = smart_cast<const CMissile*>(reaction.m_grenade);
//	if (missile && (missile->destroy_time() > Device.dwTimeGlobal))
//		interval				= missile->destroy_time() - Device.dwTimeGlobal + AFTER_GRENADE_DESTROYED_INTERVAL;
//	m_object->agent_manager().add_danger_location(reaction.m_game_object->Position(),Device.dwTimeGlobal,interval,GRENADE_RADIUS);

	if (missile && agent_manager().member().group_behaviour()) {
//		Msg						("%6d : Stalker %s : grenade reaction",Device.dwTimeGlobal,*m_object->cName());
		CEntityAlive			*initiator = smart_cast<CEntityAlive*>(Level().Objects.net_Find(reaction.m_grenade->CurrentParentID()));
		if (is_relation_enemy(initiator))
			sound().play		(StalkerSpace::eStalkerSoundGrenadeAlarm);
		else
			if (missile->Position().distance_to(Position()) < FRIENDLY_GRENADE_ALARM_DIST)
				sound().play	(StalkerSpace::eStalkerSoundFriendlyGrenadeAlarm);
	}

	reaction.clear				();
}

void CAI_Stalker::react_on_member_death	()
{
	CMemberOrder::CMemberDeathReaction	&reaction = agent_manager().member().member(this).member_death_reaction();
	if (!reaction.m_processing)
		return;

	if (Device.dwTimeGlobal < reaction.m_time + TOLLS_INTERVAL)
		return;

	if (agent_manager().member().group_behaviour())
		sound().play			(StalkerSpace::eStalkerSoundTolls);

	reaction.clear				();
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_global.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : global animations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "fooditem.h"
#include "property_storage.h"
#include "stalker_movement_manager.h"
#include "ai/stalker/ai_stalker_space.h"

using namespace StalkerSpace;

void	CStalkerAnimationManager::global_play_callback(CBlend *blend)
{
	CAI_Stalker					*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY						(object);
	if (object->animation().setup_storage()) {
		object->animation().setup_storage()->set_property(object->animation().property_id(),object->animation().property_value());
		return;
	}
	object->animation().global().make_inactual();
}

const CAnimationPair *CStalkerAnimationManager::assign_global_animation	()
{
	if ((eMentalStatePanic == object().movement().mental_state()) && !fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
		return					(&m_part_animations.A[body_state()].m_global.A[1].A[0]);

	if ((eBodyActionHello == object().body_action()))
		return					(&m_part_animations.A[eBodyStateStand].m_global.A[3].A[0]);

	CFoodItem					*food_item = smart_cast<CFoodItem*>(object().inventory().ActiveItem());
	if (!food_item)
		return					(0);

	u32							slot = food_item->animation_slot();
	switch (food_item->STATE) {
		case FOOD_HIDDEN:
		case FOOD_SHOWING:
			return				(&m_global_animations.A[slot].A[0].A[0]);
		case FOOD_HIDING :
			return				(&m_global_animations.A[slot].A[3].A[0]);
		case FOOD_PLAYING:
		case FOOD_IDLE	 :
			return				(&m_global_animations.A[slot].A[6].A[0]);
		case FOOD_PREPARE:
			return				(&m_global_animations.A[slot].A[10].A[0]);
		case FOOD_EATING :
			return				(&m_global_animations.A[slot].A[1].A[0]);
		default					: NODEFAULT;
	}
	return						(0);
}

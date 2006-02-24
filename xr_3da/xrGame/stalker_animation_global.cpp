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
#include "stalker_animation_data.h"

using namespace StalkerSpace;

void CStalkerAnimationManager::global_play_callback(CBlend *blend)
{
	CAI_Stalker						*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY							(object);
	CStalkerAnimationManager		&animation = object->animation();
	CPropertyStorage				*setup_storage = animation.setup_storage();
	if (setup_storage) {
		setup_storage->set_property	(animation.property_id(),animation.property_value());

#ifdef CLEAR_STORAGE_ON_CALLBACK
		animation.setup_storage		(0);
#endif
		return;
	}
	animation.global().make_inactual();
}

MotionID CStalkerAnimationManager::assign_global_animation	()
{
	if (eMentalStatePanic != object().movement().mental_state())
		return						(MotionID());

	if (fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
		return						(MotionID());

	return							(m_data_storage->m_part_animations.A[body_state()].m_global.A[1].A[0]);
}

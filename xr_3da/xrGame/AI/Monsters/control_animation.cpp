#include "stdafx.h"
#include "control_animation.h"
#include "BaseMonster/base_monster.h"
#include "control_manager.h"


void CControlAnimation::reinit()
{
	inherited::reinit			();

	m_data.motion.invalidate	();
	blend						= 0;
	time_started				= 0;

	default_bone_part			= smart_cast<CSkeletonAnimated*>(m_object->Visual())->LL_PartID("default");

	m_data.start_animation		= false;
}

void CControlAnimation::update_frame() 
{
	update();
}

void CControlAnimation::update() 
{
	if (m_data.start_animation) play();

	if (blend && (m_data.speed > 0)) {
		blend->speed	= m_data.speed;
	}
}

static void __stdcall animation_end_callback(CBlend* B)
{
	CControlAnimation *controller = (CControlAnimation *)B->CallbackParam;
	controller->on_animation_end();
}

void CControlAnimation::play() 
{
	VERIFY					(m_data.motion.valid());

	CSkeletonAnimated		*skeleton_animated = smart_cast<CSkeletonAnimated*>(m_object->Visual());
	blend					= skeleton_animated->LL_PlayCycle(default_bone_part, m_data.motion, TRUE, animation_end_callback, this);

	time_started			= Device.dwTimeGlobal;
	m_data.start_animation	= false;
}

void CControlAnimation::on_animation_end() 
{
	m_man->notify			(ControlCom::eventAnimationEnd, 0);
}


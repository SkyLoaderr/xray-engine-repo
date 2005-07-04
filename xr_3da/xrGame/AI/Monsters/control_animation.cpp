#include "stdafx.h"
#include "control_animation.h"
#include "BaseMonster/base_monster.h"
#include "control_manager.h"

void CControlAnimation::reinit()
{
	inherited::reinit			();

	m_skeleton_animated			= smart_cast<CSkeletonAnimated*>(m_object->Visual());
}

void CControlAnimation::reset_data()
{
	m_data.global.init	();		
	m_data.legs.init	();		
	m_data.torso.init	();
	m_data.speed		= -1.f;
}

void CControlAnimation::update_frame() 
{
	play();	
}

static void __stdcall global_animation_end_callback(CBlend* B)
{
	CControlAnimation *controller = (CControlAnimation *)B->CallbackParam;
	controller->on_global_animation_end();
}
static void __stdcall legs_animation_end_callback(CBlend* B)
{
	CControlAnimation *controller = (CControlAnimation *)B->CallbackParam;
	controller->on_legs_animation_end();
}
static void __stdcall torso_animation_end_callback(CBlend* B)
{
	CControlAnimation *controller = (CControlAnimation *)B->CallbackParam;
	controller->on_torso_animation_end();
}

void CControlAnimation::play() 
{
	if (!m_data.global.actual)
		play_part(m_data.global,	global_animation_end_callback);
	if (!m_data.legs.actual)
		play_part(m_data.legs,		legs_animation_end_callback);
	if (!m_data.torso.actual)
		play_part(m_data.torso,		torso_animation_end_callback);

	// speed only for global
	if (m_data.global.blend && (m_data.speed > 0)) {
		m_data.global.blend->speed	= m_data.speed;		// TODO: make factor
	}
}

void CControlAnimation::on_global_animation_end() 
{
	m_man->notify		(ControlCom::eventAnimationEnd, 0);
}

void CControlAnimation::on_legs_animation_end() 
{
	m_man->notify		(ControlCom::eventLegsAnimationEnd, 0);
}

void CControlAnimation::on_torso_animation_end() 
{
	m_man->notify		(ControlCom::eventTorsoAnimationEnd, 0);
}

void CControlAnimation::play_part(SAnimationPart &part, PlayCallback callback)
{
	VERIFY				(part.motion.valid());
	
	u16 bone_or_part	= m_skeleton_animated->LL_GetMotionDef(part.motion)->bone_or_part;
	if (bone_or_part == u16(-1)) bone_or_part = m_skeleton_animated->LL_PartID("default");
	
	part.blend			= m_skeleton_animated->LL_PlayCycle(bone_or_part,part.motion, TRUE, callback, this);

	part.time_started	= Device.dwTimeGlobal;
	part.actual			= true;

	m_man->notify		(ControlCom::eventAnimationStart, 0);
}

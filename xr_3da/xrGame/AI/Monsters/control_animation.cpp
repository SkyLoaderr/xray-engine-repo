#include "stdafx.h"
#include "control_animation.h"
#include "BaseMonster/base_monster.h"
#include "control_manager.h"

void CControlAnimation::reinit()
{
	inherited::reinit			();

	m_skeleton_animated			= smart_cast<CSkeletonAnimated*>(m_object->Visual());

	m_anim_events.clear			();
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
	play			();	

	check_events	(m_data.global);
	check_events	(m_data.torso);
	check_events	(m_data.legs);
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
	
	// initialize synchronization of prev and current animation
	float pos		= -1.f;
	if (part.blend && !part.blend->stop_at_end) 
		pos			= fmod(part.blend->timeCurrent,part.blend->timeTotal)/part.blend->timeTotal;

	part.blend			= m_skeleton_animated->LL_PlayCycle(bone_or_part,part.motion, TRUE, callback, this);
	
	// synchronize prev and current animations
	if ((pos > 0) && part.blend && !part.blend->stop_at_end) 
		part.blend->timeCurrent = part.blend->timeTotal*pos;

	part.time_started	= Device.dwTimeGlobal;
	part.actual			= true;

	m_man->notify		(ControlCom::eventAnimationStart, 0);

	if ((part.motion != m_data.torso.motion) && part.blend)
		m_object->CStepManager::on_animation_start(part.motion, part.blend);


	ANIMATION_EVENT_MAP_IT it = m_anim_events.find(part.motion);
	if (it != m_anim_events.end()) {
		for (ANIMATION_EVENT_VEC_IT event_it = it->second.begin(); event_it != it->second.end(); ++event_it) {
			event_it->handled = false;
		}
	}
}


void CControlAnimation::add_anim_event(MotionID motion, float time_perc, u32 id)
{
	SAnimationEvent event;
	event.time_perc = time_perc;
	event.event_id	= id;

	m_anim_events[motion].push_back(event);
}

void CControlAnimation::check_events(SAnimationPart &part)
{
	if (part.motion.valid() && part.actual && part.blend) {
		ANIMATION_EVENT_MAP_IT it = m_anim_events.find(part.motion);
		if (it != m_anim_events.end()) {

			float cur_perc = float(Device.dwTimeGlobal - part.time_started) / ((part.blend->timeTotal / part.blend->speed) * 1000);

			for (ANIMATION_EVENT_VEC_IT event_it = it->second.begin(); event_it != it->second.end(); ++event_it) {
				SAnimationEvent &event = *event_it;
				if (!event.handled && (event.time_perc < cur_perc))  {
					event.handled	= true;
						
					// gen event
					SAnimationSignalEventData	anim_event(part.motion, event.time_perc, event.event_id);
					m_man->notify				(ControlCom::eventAnimationSignal, &anim_event);
				}
			}
		}
	}
}

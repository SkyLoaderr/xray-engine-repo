////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "visual_memory_manager.h"
#include "memory_manager.h"
#include "ai/script/ai_script_monster.h"

CSoundMemoryManager::CSoundMemoryManager		()
{
	Init					();
}

CSoundMemoryManager::~CSoundMemoryManager		()
{
}

void CSoundMemoryManager::Init					()
{
	m_max_sound_count		= 0;
}

void CSoundMemoryManager::Load					(LPCSTR section)
{
	if (pSettings->line_exist(section,"DynamicSoundsCount"))
		m_max_sound_count	= pSettings->r_s32(section,"DynamicSoundsCount");
}

void CSoundMemoryManager::reinit				()
{
	m_sounds				= 0;
	m_priorities.clear		();
	m_last_sound_time		= 0;
	m_sound_threshold		= m_min_sound_threshold;
	m_sound_decrease_quant	= 250;
	m_decrease_factor		= .95f;
	m_selected_sound		= 0;
}

void CSoundMemoryManager::reload				(LPCSTR section)
{
	m_min_sound_threshold	= 0.05f;
	if (!pSettings->line_exist(section,"sound_threshold"))
		return;
	m_min_sound_threshold	= pSettings->r_float(section,"sound_threshold");
}

#define SILENCE

void CSoundMemoryManager::feel_sound_new(CObject *object, int sound_type, const Fvector &position, float sound_power)
{
	CObject					*self = dynamic_cast<CObject*>(this);
	VERIFY					(self);
#ifndef SILENCE
	Msg						("%s (%d) - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",*self->cName(),Level().timeServer(),sound_type,object ? object->cName() : "world",Level().timeServer(),position.x,position.y,position.z,sound_power);
#endif

	CScriptMonster			*script_monster = dynamic_cast<CScriptMonster*>(self);
	if (script_monster)
		script_monster->sound_callback(object,sound_type,position,sound_power);
		
	update_sound_threshold	();

	CEntityAlive			*entity_alive = dynamic_cast<CEntityAlive*>(this);
	if (entity_alive && !entity_alive->g_Alive()) {
		m_sounds->clear		();
		return;
	}
	
	sound_power				*= 1;//ffGetStartVolume(ESoundTypes(eType));
	
	if ((sound_type & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) {
		// this is fake!
		sound_power			= 1.f;
		CHitMemoryManager	*hit_memory_manager = dynamic_cast<CHitMemoryManager*>(this);
		CEntityAlive		*_entity_alive = dynamic_cast<CEntityAlive*>(object);
		if (_entity_alive && hit_memory_manager && (self->ID() != _entity_alive->ID()) && (_entity_alive->g_Team() != entity_alive->g_Team()))
			hit_memory_manager->add_hit_object(_entity_alive);
	}
	
//	if (sound_power >= m_sound_threshold)
		add_sound_object	(object,sound_type,position,sound_power);

	m_last_sound_time		= Level().timeServer();
	m_sound_threshold		= _max(m_sound_threshold,sound_power);
}

void CSoundMemoryManager::add_sound_object(const CObject *object, int sound_type, const Fvector &position, float sound_power)
{
	CObject					*self_object = dynamic_cast<CObject*>(this);
	VERIFY					(self_object);

	// we do not want to save our own sounds
	if (object && (self_object->ID() == object->ID()))
		return;

	// we do not want to save the sounds which was from the items we own
	if (object && object->H_Parent() && (object->H_Parent()->ID() == self_object->ID()))
		return;

	// we do not want to save sounds from the non-alive objects (?!)
	CMemoryManager	*memory_manager = dynamic_cast<CMemoryManager*>(this);

	//if (object && memory_manager && !memory_manager->enemy() && !dynamic_cast<CEntityAlive*>(object->H_Parent()))
	//return;
	if (object && memory_manager && !memory_manager->enemy() && !dynamic_cast<const CEntityAlive*>(object))
		return;

	// we do not want to save sounds from the teammates items
	CEntityAlive	*me				= dynamic_cast<CEntityAlive*>(this);
	if (object && object->H_Parent() && (dynamic_cast<const CEntityAlive*>(object->H_Parent())->g_Team() == me->g_Team()))
		return;

	// we do not want ot save sounds from the teammates
	const CEntityAlive	*entity_alive	= dynamic_cast<const CEntityAlive*>(object);
	if (entity_alive && me && (entity_alive->g_Team() == me->g_Team()))
		return;

	// we do not save sounds from the objects we see (?!)
	CVisualMemoryManager	*visual_memory_manager = dynamic_cast<CVisualMemoryManager*>(this);
	if (visual_memory_manager) {
		xr_vector<CVisibleObject>::const_iterator I = std::find(visual_memory_manager->memory_visible_objects().begin(),visual_memory_manager->memory_visible_objects().end(),object_id(object));
		if ((visual_memory_manager->memory_visible_objects().end() != I) && ((*I).m_level_time > m_last_sound_time))
			return;
	}

	const CGameObject *game_object	= dynamic_cast<const CGameObject*>(object);
	CGameObject *self			= dynamic_cast<CGameObject*>(this);
	if (!game_object || (visual_memory_manager && !visual_memory_manager->visible_now(game_object)))
		return;

#ifndef SILENCE
	Msg							("* %s - ref_sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",*self->cName(),sound_type,object ? object->cName() : "world",Level().timeServer(),position.x,position.y,position.z,sound_power);
#endif

	xr_vector<CSoundObject>::iterator	J = std::find(m_sounds->begin(),m_sounds->end(),object_id(object));
	if (m_sounds->end() == J) {
		CSoundObject			sound_object;

		sound_object.fill		(game_object,self,ESoundTypes(sound_type),sound_power);
		sound_object.m_first_level_time	= Level().timeServer();
		sound_object.m_first_game_time	= Level().GetGameTime();

		VERIFY					(m_max_sound_count);
		if (m_max_sound_count <= m_sounds->size()) {
			xr_vector<CSoundObject>::iterator	I = std::min_element(m_sounds->begin(),m_sounds->end(),SLevelTimePredicate<CGameObject>());
			VERIFY				(m_sounds->end() != I);
			*I					= sound_object;
		}
		else
			m_sounds->push_back	(sound_object);
	}
	else
		(*J).fill				(game_object,self,ESoundTypes(sound_type),sound_power);
}

void CSoundMemoryManager::update()
{
	m_selected_sound			= 0;
	u32							priority = u32(-1);
	xr_vector<CSoundObject>::const_iterator	I = m_sounds->begin();
	xr_vector<CSoundObject>::const_iterator	E = m_sounds->end();
	for ( ; I != E; ++I) {
		u32						cur_priority = get_priority(*I);
		if (cur_priority < priority) {
			m_selected_sound	= &*I;
			priority			= cur_priority;
		}
	}
}

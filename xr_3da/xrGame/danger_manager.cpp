////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_manager.cpp
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "danger_manager.h"
#include "custommonster.h"
#include "memory_space.h"
#include "profiler.h"

struct CDangerPredicate {
	const CObject	*m_object;

	IC			CDangerPredicate	(const CObject *object)
	{
		m_object			= object;
	}

	IC	bool	operator()			(const CDangerObject &object) const
	{
		if (!m_object)
			return			(!object.object());

		if (!object.object())
			return			(false);

		return				(object.object()->ID() == m_object->ID());
	}
};

struct CFindPredicate {
	const CDangerObject		*m_object;

	IC			CFindPredicate		(const CDangerObject &object)
	{
		m_object			= &object;
	}

	IC	bool	operator()			(const CDangerObject &object) const
	{
		return				(*m_object == object);
	}
};

struct CRemoveByTimePredicate {
	u32			m_time_line;

	IC			CRemoveByTimePredicate	(u32 time_line)
	{
		m_time_line			= time_line;
	}

	IC	bool	operator()				(const CDangerObject &object) const
	{
		return				(object.time() < m_time_line);
	}
};

CDangerManager::~CDangerManager		()
{
}

void CDangerManager::Load			(LPCSTR section)
{
}

void CDangerManager::reinit			()
{
	m_objects.clear			();
	m_time_line				= 0;
}

void CDangerManager::reload			(LPCSTR section)
{
}

void CDangerManager::update			()
{
	START_PROFILE("AI/Memory Manager/dangers/update")

	{
		OBJECTS::iterator	I = remove_if(m_objects.begin(),m_objects.end(),CRemoveByTimePredicate(time_line()));
		m_objects.erase		(I,m_objects.end());
	}

	float					result = flt_max;
	m_selected				= 0;
	OBJECTS::const_iterator	I = m_objects.begin();
	OBJECTS::const_iterator	E = m_objects.end();
	for ( ; I != E; ++I) {
//		Msg					("%6d : Danger : [%d][%d]",(*I).time(),(*I).type(),(*I).perceive_type());
		float				value = do_evaluate(*I);
		if (result > value) {
			result			= value;
			m_selected		= &*I;
		}
	}

	STOP_PROFILE
}

void CDangerManager::remove_links	(const CObject *object)
{
	OBJECTS::iterator		I = remove_if(m_objects.begin(),m_objects.end(),CDangerPredicate(object));
	m_objects.erase			(I,m_objects.end());
	m_selected				= 0;
}

bool CDangerManager::useful			(const CDangerObject &object) const
{
	return					(object.time() >= time_line());
}

bool CDangerManager::is_useful		(const CDangerObject &object) const
{
	return					(m_object->useful(this,object));
}

float CDangerManager::evaluate		(const CDangerObject &object) const
{
	return					(m_object->evaluate(this,object));
}

float CDangerManager::do_evaluate	(const CDangerObject &object) const
{
	float					result = 0.f;
	switch (object.type()) {
		case CDangerObject::eDangerTypeRicochet : {
			result			+= 3000.f;
			break;
		}
		case CDangerObject::eDangerTypeShot : {
			result			+= 2500.f;
			break;
												}
		case CDangerObject::eDangerTypeHit : {
			result			+= 2000.f;
			break;
		}
		case CDangerObject::eDangerTypeDeath : {
			result			+= 1000.f;
			break;
		}
		case CDangerObject::eDangerTypeAttack : {
			result			+= 2000.f;
			break;
		}
		case CDangerObject::eDangerTypeCorpse : {
			result			+= 2250.f;
			break;
		}
		default : NODEFAULT;
	}

	result					+= object.position().distance_to(m_object->Position());

	return					(result);
}

void CDangerManager::add			(const CVisibleObject &object)
{
	if (!object.m_enabled)
		return;

	const CEntityAlive		*obj = smart_cast<const CEntityAlive*>(object.m_object);
	if (obj && !obj->g_Alive() && obj->killer_id() != ALife::_OBJECT_ID(-1)) {
		add					(CDangerObject(obj,obj->Position(),object.m_level_time,CDangerObject::eDangerTypeCorpse,CDangerObject::eDangerPerceiveTypeSound));
		return;
	}
}

void CDangerManager::add			(const CSoundObject &object)
{
	if (!object.m_enabled)
		return;
	
	const CEntityAlive		*obj = smart_cast<const CEntityAlive*>(object.m_object);
	
	if (object.m_sound_type & SOUND_TYPE_BULLET_HIT) {
		add					(CDangerObject(obj,object.m_object_params.m_position,object.m_level_time,CDangerObject::eDangerTypeRicochet,CDangerObject::eDangerPerceiveTypeSound));
		return;
	}

	if (object.m_sound_type & SOUND_TYPE_WEAPON_SHOOTING) {
		add					(CDangerObject(obj,object.m_object_params.m_position,object.m_level_time,CDangerObject::eDangerTypeShot,CDangerObject::eDangerPerceiveTypeSound));
		return;
	}

	if (object.m_sound_type & SOUND_TYPE_INJURING) {
		add					(CDangerObject(obj,object.m_object_params.m_position,object.m_level_time,CDangerObject::eDangerTypeHit,CDangerObject::eDangerPerceiveTypeSound));
		return;
	}

	if (object.m_sound_type & SOUND_TYPE_DYING) {
		add					(CDangerObject(obj,object.m_object_params.m_position,object.m_level_time,CDangerObject::eDangerTypeDeath,CDangerObject::eDangerPerceiveTypeSound));
		return;
	}
}

void CDangerManager::add			(const CHitObject &object)
{
	if (!object.m_enabled)
		return;

	if (fis_zero(object.m_amount))
		return;

	const CEntityAlive		*obj = smart_cast<const CEntityAlive*>(object.m_object);
	add						(CDangerObject(obj,obj->Position(),object.m_level_time,CDangerObject::eDangerTypeAttack,CDangerObject::eDangerPerceiveTypeHit));
}

void CDangerManager::add			(const CDangerObject &object)
{
	if (!is_useful(object))
		return;

	OBJECTS::iterator		I = std::find_if(m_objects.begin(),m_objects.end(),CFindPredicate(object));
	if (I != m_objects.end()) {
		*I					= object;
		return;
	}

	m_objects.push_back		(object);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_memory_manager.h"
#include "custommonster.h"

struct CHitObjectPredicate {
	const CObject *m_object;
	CHitObjectPredicate			(const CObject *object) :
	m_object(object)
	{
	}

	bool		operator()					(const CHitObject &hit_object) const
	{
		if (!m_object)
			return			(!hit_object.m_object);
		if (!hit_object.m_object)
			return			(false);
		return				(m_object->ID() == hit_object.m_object->ID());
	}
};

CHitMemoryManager::CHitMemoryManager		()
{
	init						();
}

CHitMemoryManager::~CHitMemoryManager		()
{
}

void CHitMemoryManager::init				()
{
}

void CHitMemoryManager::Load				(LPCSTR section)
{
	m_object				= dynamic_cast<CCustomMonster*>(this);
	VERIFY					(m_object);
	if (pSettings->line_exist(section,"HurtCount"))
		m_max_hit_count	= pSettings->r_s32(section,"HurtCount");
}

bool CHitMemoryManager::hit				(const CEntityAlive *object) const
{
	VERIFY				(m_hits);
	xr_vector<CHitObject>::const_iterator	I = std::find_if(m_hits->begin(),m_hits->end(),CHitObjectPredicate(object));
	return				(m_hits->end() != I);
}

void CHitMemoryManager::add_hit_object	(const CEntityAlive *entity_alive)
{
	add_hit_object		(0,Fvector().set(0,0,1),entity_alive,0);
}

void CHitMemoryManager::reinit				()
{
	m_hits					= 0;
	m_max_hit_count			= 1;
	m_selected_hit			= 0;
}

void CHitMemoryManager::reload				(LPCSTR section)
{
}

void CHitMemoryManager::add_hit_object		(float amount, const Fvector &vLocalDir, const CObject *who, s16 element)
{
	VERIFY						(m_hits);
	CEntityAlive				*self = m_object;
	if (!self->g_Alive()) {
		m_hits->clear			();
		return;
	}

	if (who && (self->ID() == who->ID()))
		return;

	CScriptMonster			*script_monster = m_object;
	script_monster->hit_callback(amount,vLocalDir,who,element);

	//.
	CObject						*object = m_object;
	Fvector						direction;
	object->XFORM().transform_dir(direction,vLocalDir);

	const CEntityAlive			*entity_alive = dynamic_cast<const CEntityAlive*>(who);
	if (!entity_alive || (self->tfGetRelationType(entity_alive) == ALife::eRelationTypeFriend))
		return;

	xr_vector<CHitObject>::iterator	J = std::find(m_hits->begin(),m_hits->end(),object_id(who));
	if (m_hits->end() == J) {
		CHitObject				hit_object;

		hit_object.fill			(entity_alive,self);
		hit_object.m_first_level_time	= Level().timeServer();
		hit_object.m_first_game_time	= Level().GetGameTime();

		if (m_max_hit_count <= m_hits->size()) {
			xr_vector<CHitObject>::iterator	I = std::min_element(m_hits->begin(),m_hits->end(),SLevelTimePredicate<CEntityAlive>());
			VERIFY				(m_hits->end() != I);
			*I					= hit_object;
		}
		else
			m_hits->push_back	(hit_object);
	}
	else
		(*J).fill				(entity_alive,self);
}

void CHitMemoryManager::add_hit_object(const CHitObject &hit_object)
{
	VERIFY						(m_hits);
	CEntityAlive				*self = m_object;
	if (!self->g_Alive()) {
		m_hits->clear			();
		return;
	}

	const CEntityAlive			*entity_alive = hit_object.m_object;
	xr_vector<CHitObject>::iterator	J = std::find(m_hits->begin(),m_hits->end(),object_id(entity_alive));
	if (m_hits->end() == J) {
		if (m_max_hit_count <= m_hits->size()) {
			xr_vector<CHitObject>::iterator	I = std::min_element(m_hits->begin(),m_hits->end(),SLevelTimePredicate<CEntityAlive>());
			VERIFY				(m_hits->end() != I);
			*I					= hit_object;
		}
		else
			m_hits->push_back	(hit_object);
	}
	else
		*J						= hit_object;
}

struct CRemoveOfflinePredicate {
	bool		operator()						(const CHitObject &object) const
	{
		VERIFY	(object.m_object);
		return	(!!object.m_object->getDestroy() || object.m_object->H_Parent());
	}
};

void CHitMemoryManager::update()
{
	VERIFY						(m_hits);
	{
		xr_vector<CHitObject>::iterator	I = remove_if(m_hits->begin(),m_hits->end(),CRemoveOfflinePredicate());
		m_hits->erase					(I,m_hits->end());
	}

	m_selected_hit				= 0;
	u32							level_time = 0;
	xr_vector<CHitObject>::const_iterator	I = m_hits->begin();
	xr_vector<CHitObject>::const_iterator	E = m_hits->end();
	for ( ; I != E; ++I) {
		if ((*I).m_level_time > level_time) {
			m_selected_hit		= &*I;
			level_time			= (*I).m_level_time;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_memory_manager.h"

CHitMemoryManager::CHitMemoryManager		()
{
	Init						();
}

CHitMemoryManager::~CHitMemoryManager		()
{
}

void CHitMemoryManager::Init				()
{
}

void CHitMemoryManager::Load				(LPCSTR section)
{
	if (pSettings->line_exist(section,"HurtCount"))
		m_max_hit_count	= pSettings->r_s32(section,"HurtCount");
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

void CHitMemoryManager::add_hit_object		(float amount, const Fvector &vLocalDir, CObject *who, s16 element)
{
	CEntityAlive				*self = dynamic_cast<CEntityAlive*>(this);
	if (self && !self->g_Alive()) {
		m_hits->clear			();
		return;
	}

	CObject						*object = dynamic_cast<CObject*>(this);
	VERIFY						(object);
	Fvector						direction;
	object->XFORM().transform_dir(direction,vLocalDir);

	CEntityAlive				*entity_alive = dynamic_cast<CEntityAlive*>(who);
	xr_vector<CHitObject>::iterator	J = std::find(m_hits->begin(),m_hits->end(),who->ID());
	if (m_hits->end() == J) {
		CHitObject				hit_object;

		hit_object.fill			(entity_alive,self);

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
	CEntityAlive				*self = dynamic_cast<CEntityAlive*>(this);
	if (self && !self->g_Alive()) {
		m_hits->clear			();
		return;
	}

	CEntityAlive				*entity_alive = hit_object.m_object;
	xr_vector<CHitObject>::iterator	J = std::find(m_hits->begin(),m_hits->end(),entity_alive->ID());
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

void CHitMemoryManager::update()
{
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

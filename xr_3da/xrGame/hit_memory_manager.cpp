////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_memory_manager.h"
#include "memory_space_impl.h"
#include "custommonster.h"
#include "ai_object_location.h"
#include "level_graph.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "game_object_space.h"
#include "profiler.h"

struct CHitObjectPredicate {
	const CObject *m_object;

				CHitObjectPredicate			(const CObject *object) :
					m_object				(object)
	{
	}

	bool		operator()					(const MemorySpace::CHitObject &hit_object) const
	{
		if (!m_object)
			return			(!hit_object.m_object);

		if (!hit_object.m_object)
			return			(false);

		return				(m_object->ID() == hit_object.m_object->ID());
	}
};

CHitMemoryManager::~CHitMemoryManager		()
{
#ifdef USE_SELECTED_HIT
	xr_delete				(m_selected_hit);
#endif
}

const CHitObject *CHitMemoryManager::hit					(const CEntityAlive *object) const
{
	VERIFY					(m_hits);
	HITS::const_iterator	I = std::find_if(m_hits->begin(),m_hits->end(),CHitObjectPredicate(object));
	if (m_hits->end() != I)
		return				(&*I);

	return					(0);
}

void CHitMemoryManager::add					(const CEntityAlive *entity_alive)
{
	add						(0,Fvector().set(0,0,1),entity_alive,0);
}

void CHitMemoryManager::Load				(LPCSTR section)
{
}

void CHitMemoryManager::reinit				()
{
}

void CHitMemoryManager::reload				(LPCSTR section)
{
	m_hits					= 0;
#ifdef USE_SELECTED_HIT
	xr_delete				(m_selected_hit);
#endif
	m_max_hit_count			= READ_IF_EXISTS(pSettings,r_s32,section,"DynamicHitCount",1);
}

void CHitMemoryManager::add					(float amount, const Fvector &vLocalDir, const CObject *who, s16 element)
{
	VERIFY						(m_hits);
	if (!object().g_Alive())
		return;

	if (who && (m_object->ID() == who->ID()))
		return;

	object().callback(GameObject::eHit)(
		m_object->lua_game_object(), 
		amount,
		vLocalDir,
		smart_cast<const CGameObject*>(who)->lua_game_object(),
		element
	);

	Fvector						direction;
	m_object->XFORM().transform_dir	(direction,vLocalDir);

	const CEntityAlive			*entity_alive = smart_cast<const CEntityAlive*>(who);
	if (!entity_alive || (m_object->tfGetRelationType(entity_alive) == ALife::eRelationTypeFriend))
		return;

	HITS::iterator				J = std::find(m_hits->begin(),m_hits->end(),object_id(who));
	if (m_hits->end() == J) {
		CHitObject						hit_object;

		hit_object.fill					(entity_alive,m_object,!m_stalker ? squad_mask_type(-1) : m_stalker->agent_manager().member().mask(m_stalker));
		
#ifdef USE_FIRST_GAME_TIME
		hit_object.m_first_game_time	= Level().GetGameTime();
#endif
#ifdef USE_FIRST_LEVEL_TIME
		hit_object.m_first_level_time	= Device.dwTimeGlobal;
#endif
		hit_object.m_amount				= amount;

		if (m_max_hit_count <= m_hits->size()) {
			HITS::iterator		I = std::min_element(m_hits->begin(),m_hits->end(),SLevelTimePredicate<CEntityAlive>());
			VERIFY				(m_hits->end() != I);
			*I					= hit_object;
		}
		else
			m_hits->push_back	(hit_object);
	}
	else {
		(*J).fill				(entity_alive,m_object,(!m_stalker ? (*J).m_squad_mask.get() : ((*J).m_squad_mask.get() | m_stalker->agent_manager().member().mask(m_stalker))));
		(*J).m_amount			= _max(amount,(*J).m_amount);
	}
}

void CHitMemoryManager::add					(const CHitObject &_hit_object)
{
	VERIFY						(m_hits);
	if (!object().g_Alive())
		return;

	CHitObject					hit_object = _hit_object;
	hit_object.m_squad_mask.set	(m_stalker->agent_manager().member().mask(m_stalker),TRUE);

	const CEntityAlive			*entity_alive = hit_object.m_object;
	HITS::iterator	J = std::find(m_hits->begin(),m_hits->end(),object_id(entity_alive));
	if (m_hits->end() == J) {
		if (m_max_hit_count <= m_hits->size()) {
			HITS::iterator	I = std::min_element(m_hits->begin(),m_hits->end(),SLevelTimePredicate<CEntityAlive>());
			VERIFY				(m_hits->end() != I);
			*I					= hit_object;
		}
		else
			m_hits->push_back	(hit_object);
	}
	else {
		hit_object.m_squad_mask.assign	(hit_object.m_squad_mask.get() | (*J).m_squad_mask.get());
		*J						= hit_object;
	}
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
	START_PROFILE("Memory Manager/hits::update")

	VERIFY						(m_hits);
	{
		HITS::iterator			I = remove_if(m_hits->begin(),m_hits->end(),CRemoveOfflinePredicate());
		m_hits->erase			(I,m_hits->end());
	}

#ifdef USE_SELECTED_HIT
	xr_delete					(m_selected_hit);
	u32							level_time = 0;
	HITS::const_iterator		I = m_hits->begin();
	HITS::const_iterator		E = m_hits->end();
	for ( ; I != E; ++I) {
		if ((*I).m_level_time > level_time) {
			xr_delete			(m_selected_hit);
			m_selected_hit		= xr_new<CHitObject>(*I);
			level_time			= (*I).m_level_time;
		}
	}
#endif
	STOP_PROFILE
}

void CHitMemoryManager::enable			(const CObject *object, bool enable)
{
	HITS::iterator	J = std::find(m_hits->begin(),m_hits->end(),object_id(object));
	if (J == m_hits->end())
		return;
	(*J).m_enabled		= enable;
}

void CHitMemoryManager::remove_links	(CObject *object)
{
	VERIFY				(m_hits);
	HITS::iterator		I = std::find_if(m_hits->begin(),m_hits->end(),CHitObjectPredicate(object));
	if (I != m_hits->end())
		m_hits->erase	(I);

#ifdef USE_SELECTED_HIT
	if (!m_selected_hit)
		return;

	if (!m_selected_hit->m_object)
		return;

	if (m_selected_hit->m_object->ID() != object->ID())
		return;

	xr_delete			(m_selected_hit);
#endif
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_space_impl.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Memory space implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "memory_space.h"
#include "gameobject.h"
#include "level.h"
#include "ai_space.h"

template <typename T>
IC	SRotation CObjectParams<T>::orientation	(const T *object) const
{
	Fvector					t;
	object->XFORM().getHPB	(t.x,t.y,t.z);
	return					(SRotation(t.x,t.y,0.f));
}

template <typename T>
IC	void CObjectParams<T>::fill				(const T *game_object)
{
	m_orientation			= game_object ? orientation(game_object) : SRotation(0.f,0.f,0.f);
	m_level_vertex_id		= game_object ? game_object->ai_location().level_vertex_id() : u32(-1);
	if (game_object && ai().get_level_graph() && ai().level_graph().valid_vertex_id(m_level_vertex_id) && !ai().level_graph().inside(m_level_vertex_id,game_object->Position())) {
		m_position			= ai().level_graph().vertex_position(m_level_vertex_id);
		return;
	}
	if (game_object) {
		game_object->Center	(m_position);
		m_position.set		(game_object->Position().x,m_position.y,game_object->Position().z);
	}
	else {
		m_position			= Fvector().set(0.f,0.f,0.f);
	}
}

template <typename T>
IC	CMemoryObject<T>::CMemoryObject			()
{
	m_squad_mask.one		();
	m_object				= 0;
}

template <typename T>
IC	bool CMemoryObject<T>::operator==		(u16 id) const
{
	return					(object_id(m_object) == id);
}

template <typename T>
IC	void CMemoryObject<T>::fill				(const T *game_object, const T *self, const squad_mask_type &mask)
{
	++m_update_count;
	m_object				= game_object;
	m_last_game_time		= m_game_time;
	m_last_level_time		= m_level_time;
	m_game_time				= Level().GetGameTime();
	m_level_time			= Device.dwTimeGlobal;
	m_object_params.fill	(game_object);
	m_self_params.fill		(self);
	m_squad_mask.assign		(mask);
	SMemoryObject::fill		();
}

template <typename T>
IC	u16 object_id(const T *object)
{
	return					(object ? u16(object->ID()) : u16(0xffff));
}

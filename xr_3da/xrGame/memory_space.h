////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_space.h
//	Created 	: 25.12.2003
//  Modified 	: 25.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Mmemory space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "entity_alive.h"

namespace MemorySpace {
	struct SObjectParams {
		u32							m_level_vertex_id;
		Fvector						m_position;
		SRotation					m_orientation;
	};
	
	template <typename T>
	struct CObjectParams : public SObjectParams {
		IC		SRotation orientation				(const T *object) const
		{
			Fvector					t;
			object->XFORM().getHPB	(t.x,t.y,t.z);
			return					(SRotation(t.x,t.y));
		}
		
		IC		void	fill						(const T *game_object)
		{
			m_level_vertex_id		= game_object->level_vertex_id();
			m_position				= game_object->Position();
			m_orientation			= orientation(game_object);
		}
	};

	struct SMemoryObject {
		ALife::_TIME_ID				m_game_time;
		u32							m_level_time;
		u32							m_update_count;

						SMemoryObject				() : m_update_count(0)
		{
		}
	};

	template <typename T>
	struct CMemoryObject : public SMemoryObject {
		T							*m_object;
		CObjectParams<T>			m_object_params;
		CObjectParams<T>			m_self_params;

		IC	bool	operator==						(u32 id) const
		{
			VERIFY					(m_object);
			return					(m_object->ID() == id);
		}

		IC	void	fill							(T *game_object, T *self)
		{
			++m_update_count;
			m_object				= game_object;
			m_game_time				= Level().GetGameTime();
			m_level_time			= Level().timeServer();
			m_object_params.fill	(game_object);
			m_self_params.fill		(self);
		}
	};

	struct CVisibleObject : CMemoryObject<CGameObject> {
		typedef CMemoryObject<CGameObject> inherited;
		bool						m_visible;

		bool	visible				() const 
		{
			return					(m_visible);
		}

		IC	void	fill							(CGameObject *game_object, CGameObject *self)
		{
			inherited::fill			(game_object,self);
			m_visible				= true;
		}
	};

	struct CHitObject : public CMemoryObject<CEntityAlive> {
		Fvector						m_direction;
		u16							m_bone_index;
		float						m_amount;
	};
	
	struct CSoundObject : public CVisibleObject {
		ESoundTypes					m_sound_type;
		float						m_power;

		IC	void	fill							(CGameObject *game_object, CGameObject *self, const ESoundTypes sound_type, const float sound_power)
		{
			CVisibleObject::fill	(game_object,self);
			m_sound_type			= sound_type;
			m_power					= sound_power;
		}
	};

	template <typename T>
	struct SLevelTimePredicate {
		bool		operator()						(const CMemoryObject<T> &object1, const CMemoryObject<T> &object2) const
		{
			VERIFY					(object1.m_object);
			VERIFY					(object2.m_object);
			return					(object1.m_level_time < object2.m_level_time);
		}
	};
};

using namespace MemorySpace;
////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_space.h
//	Created 	: 25.12.2003
//  Modified 	: 25.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_sounds.h"
#include "xrserver_space.h"
#include "alife_space.h"
#include "script_export_space.h"

class CEntityAlive;
class CGameObject;

namespace MemorySpace {
	typedef u32 squad_mask_type;

	struct CNotYetVisibleObject {
		const CGameObject			*m_object;
		float						m_value;
		bool						m_updated;
		u32							m_prev_time;
	};

	struct SObjectParams {
		u32							m_level_vertex_id;
		Fvector						m_position;
		SRotation					m_orientation;
	};
	
	template <typename T>
	struct CObjectParams : public SObjectParams {
		IC		SRotation orientation	(const T *object) const;
		IC		void	fill			(const T *game_object);
	};

	struct SMemoryObject {
		ALife::_TIME_ID				m_game_time;
		u32							m_level_time;
		ALife::_TIME_ID				m_last_game_time;
		u32							m_last_level_time;
		ALife::_TIME_ID				m_first_game_time;
		u32							m_first_level_time;
		u32							m_update_count;
		bool						m_enabled;

						SMemoryObject				() : 
							m_update_count(0),
							m_game_time(0),
							m_level_time(0),
							m_last_game_time(0),
							m_last_level_time(0),
							m_first_game_time(0),
							m_first_level_time(0),
							m_enabled(true)
		{
		}

		IC		void	fill						()
		{
			m_enabled				= true;
		}
	};

	template <typename T>
	struct CMemoryObject : public SMemoryObject {
		const T						*m_object;
		CObjectParams<T>			m_object_params;
		CObjectParams<T>			m_self_params;
		_flags<squad_mask_type>		m_squad_mask;

		IC			CMemoryObject	();
		IC	bool	operator==		(u16 id) const;
		IC	void	fill			(const T *game_object, const T *self, const squad_mask_type &mask);
	};

	struct CVisibleObject : CMemoryObject<CGameObject> {
		typedef CMemoryObject<CGameObject> inherited;
		bool						m_visible;

		bool	visible				() const 
		{
			return					(m_visible);
		}

		IC	void	fill							(const CGameObject *game_object, const CGameObject *self, const squad_mask_type &mask)
		{
			inherited::fill			(game_object,self,mask);
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

		IC	void	fill							(const CGameObject *game_object, const CGameObject *self, const ESoundTypes sound_type, const float sound_power, const squad_mask_type &mask)
		{
			CVisibleObject::fill	(game_object,self,mask);
			m_sound_type			= sound_type;
			m_power					= sound_power;
		}

		IC	int		sound_type		() const
		{
			return					(int(m_sound_type));
		}
	};

	struct CMemoryInfo : public CVisibleObject {
		bool						m_visual_info;
		bool						m_sound_info;
		bool						m_hit_info;
					
					CMemoryInfo		()
		{
			m_visual_info			= false;
			m_sound_info			= false;
			m_hit_info				= false;
		}
		DECLARE_SCRIPT_REGISTER_FUNCTION
	};
	add_to_type_list(CMemoryInfo)
	#undef script_type_list
	#define script_type_list save_type_list(CMemoryInfo)

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

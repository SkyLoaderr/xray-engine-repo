////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../feel_vision.h"
#include "memory_space_impl.h"

class CVisualMemory : public Feel::Vision {
protected:
	xr_vector<CObject*>	m_visible_objects;
	friend class CScriptMonster;
};

class CAI_Stalker;
class CCustomMonster;

class CVisualMemoryManager : 
	virtual public CGameObject,
	public CVisualMemory
{
public:
	struct SRemoveOfflinePredicate {
		bool		operator()						(const CVisibleObject &object) const
		{
			VERIFY	(object.m_object);
			return	(!!object.m_object->getDestroy() || object.m_object->H_Parent());
		}
		
		bool		operator()						(const CNotYetVisibleObject &object) const
		{
			VERIFY	(object.m_object);
			return	(!!object.m_object->getDestroy() || object.m_object->H_Parent());
		}
	};

	struct CVisibleObjectPredicate {
		u32			m_id;
					CVisibleObjectPredicate			(u32 id) : 
						m_id(id)
		{
		}

		bool		operator()						(const CObject *object) const
		{
			VERIFY	(object);
			return	(object->ID() == m_id);
		}
	};

	struct CVisionParameters {
		float						m_min_view_distance;
		float						m_max_view_distance;
		float						m_visibility_threshold;
		float						m_always_visible_distance;
		float						m_time_quant;
		float						m_decrease_value;
		float						m_velocity_factor;
		float						m_transparency_threshold;
		float						m_luminocity_factor;

			void	Load			(LPCSTR section, bool not_a_stalker);
	};

private:
	u32								m_max_object_count;
	xr_vector<CVisibleObject>		*m_objects;
	xr_vector<CNotYetVisibleObject>	m_not_yet_visible_objects;
	CVisionParameters				m_free;
	CVisionParameters				m_danger;
	bool							m_enabled;

private:
	// to minimize dynamic_casts
	CCustomMonster					*m_monster;
	CAI_Stalker						*m_stalker;

protected:
	IC		const CVisionParameters &current_state	() const;
	IC		void	fill_object						(CVisibleObject &visible_object, const CGameObject *game_object);
			void	add_visible_object				(const CObject *object, float time_delta);
			void	add_visible_object				(const CVisibleObject visible_object);
	virtual	void	update							(float time_delta);
			float	object_visible_distance			(const CGameObject *game_object, float &object_distance) const;
			float	object_luminocity				(const CGameObject *game_object) const;
			float	get_visible_value				(float distance, float object_distance, float time_delta, float object_velocity, float luminocity) const;
			float	get_object_velocity				(const CGameObject *game_object, const CNotYetVisibleObject &not_yet_visible_object) const;
			u32		get_prev_time					(const CGameObject *game_object) const;
			void	add_not_yet_visible_object		(const CNotYetVisibleObject &not_yet_visible_object);
			CNotYetVisibleObject *not_yet_visible_object(const CGameObject *game_object);

public:
					CVisualMemoryManager			();
	virtual			~CVisualMemoryManager			();
			void	init							();
	virtual	void	Load							(LPCSTR section);
	virtual	void	reinit							();
	virtual	void	reload							(LPCSTR section);
	IC		const xr_vector<CVisibleObject>	&memory_visible_objects() const;
	IC		const xr_vector<CObject*>	&visible_objects() const;
	IC		const xr_vector<CNotYetVisibleObject> &not_yet_visible_objects() const;
	IC		const u32 visible_object_time_last_seen(const CObject *object) const;
			bool	visible							(const CGameObject *game_object, float time_delta);
			bool	visible							(u32 level_vertex_id, float yaw, float eye_fov) const;
	IC		void	set_squad_objects				(xr_vector<CVisibleObject> *squad_objects);
	IC		bool	visible_now						(const CGameObject *game_object) const;
	IC		void	enable							(const CObject *object, bool enable);
	IC		float	visibility_threshold			() const;
	virtual	float	feel_vision_mtl_transp			(u32 element);	
	IC		float	transparency_threshold			() const;
	IC		bool	enabled							() const;
	IC		void	enable							(bool value);
};

#include "visual_memory_manager_inline.h"
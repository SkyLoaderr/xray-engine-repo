////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "visual_memory_params.h"
#include "memory_space.h"

class	CCustomMonster;
class	CAI_Stalker;

class CVisualMemoryManager {
#ifdef DEBUG
	friend class CAI_Stalker;
#endif
public:
	typedef MemorySpace::CVisibleObject			CVisibleObject;
	typedef MemorySpace::CNotYetVisibleObject	CNotYetVisibleObject;
	typedef xr_vector<CVisibleObject>			VISIBLES;
	typedef xr_vector<CObject*>					RAW_VISIBLES;
	typedef xr_vector<CNotYetVisibleObject>		NOT_YET_VISIBLES;

private:
	CCustomMonster		*m_object;
	CAI_Stalker			*m_stalker;

private:
	RAW_VISIBLES		m_visible_objects;
	VISIBLES			*m_objects;
	NOT_YET_VISIBLES	m_not_yet_visible_objects;

private:
	CVisionParameters	m_free;
	CVisionParameters	m_danger;

private:
	u32					m_max_object_count;
	bool				m_enabled;

protected:
	IC		void	fill_object				(CVisibleObject &visible_object, const CGameObject *game_object);
			void	add_visible_object		(const CObject *object, float time_delta);
			void	add_visible_object		(const CVisibleObject visible_object);
			float	object_visible_distance	(const CGameObject *game_object, float &object_distance) const;
			float	object_luminocity		(const CGameObject *game_object) const;
			float	get_visible_value		(float distance, float object_distance, float time_delta, float object_velocity, float luminocity) const;
			float	get_object_velocity		(const CGameObject *game_object, const CNotYetVisibleObject &not_yet_visible_object) const;
			u32		get_prev_time			(const CGameObject *game_object) const;

protected:
	IC		u32		visible_object_time_last_seen			(const CObject *object) const;
	IC		const CVisionParameters &current_state			() const;
			void	add_not_yet_visible_object				(const CNotYetVisibleObject &not_yet_visible_object);
			CNotYetVisibleObject *not_yet_visible_object	(const CGameObject *game_object);

public:
					CVisualMemoryManager	(CCustomMonster *object);
	virtual			~CVisualMemoryManager	();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);
	virtual	void	update					(float time_delta);
	virtual	float	feel_vision_mtl_transp	(u32 element);	
			void	remove_links			(CObject *object);

public:
			bool	visible					(const CGameObject *game_object, float time_delta);
			bool	visible					(u32 level_vertex_id, float yaw, float eye_fov) const;

public:
	IC		void	set_squad_objects		(xr_vector<CVisibleObject> *squad_objects);
			bool	visible_now				(const CGameObject *game_object) const;
			void	enable					(const CObject *object, bool enable);

public:
	IC		float	visibility_threshold	() const;
	IC		float	transparency_threshold	() const;

public:
	IC		bool	enabled					() const;
	IC		void	enable					(bool value);

public:
	IC		const VISIBLES			&objects					() const;
	IC		const RAW_VISIBLES		&raw_objects				() const;
	IC		const NOT_YET_VISIBLES	&not_yet_visible_objects	() const;
};

#include "visual_memory_manager_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_level_selector.h"

class CLevelPathManager {
protected:
	bool									m_level_path_actual;
			bool	valid					() const;
			bool	valid					(u32 node_id) const;
public:
	enum ELevelPathType {
		eLevelPathTypeMinTime = u32(0),
		eLevelPathTypeCoverLight,
		eLevelPathTypePositionDetour,
	};
	xr_vector<u32>							m_level_path;
	u32										m_level_start_node;
	u32										m_level_dest_node;
	u32										m_level_cur_node_index;
	ELevelPathType							m_level_path_type;
	float									m_max_range;
	u32										m_max_iterations;
	u32										m_max_node_visited;
	bool									m_consider_obstacles;
	float									m_cover_weight;
	float									m_light_weight;
	float									m_distance_weight;
	Fvector									m_enemy_position;
	float									m_enemy_distance;
	float									m_enemy_view;
	PathManagers::CAbstractNodeEvaluator	*m_level_path_evaluator;

					CLevelPathManager			();
	virtual			~CLevelPathManager			();
	virtual void	init						();
			void	build_level_path			();
			void	select_level_path_vertex	();
			bool	level_path_actual			() const;
			bool	level_path_completed		() const;
};
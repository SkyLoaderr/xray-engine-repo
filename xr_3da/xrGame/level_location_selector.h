////////////////////////////////////////////////////////////////////////////
//	Module 		: level_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_selector.h"
#include "gameobject.h"

class CLevelLocationSelector : virtual public CGameObject {
protected:
	struct SEnemySelected {
		CEntity*							m_enemy;
		bool								m_visible;
		float								m_cost;
	};

	Fvector									m_hit_direction;
	u32										m_hit_time;
	SEnemySelected							m_selected_enemy;
	u32										m_level_locate_query_time;
	u32										m_level_location_query_interval;
	bool									m_level_selector_failed;
	PathManagers::CAbstractNodeEvaluator	*m_level_selector_evaluator;

public:
					CLevelLocationSelector	();
	virtual			~CLevelLocationSelector	();
//	virtual void	init_evaluator			(PathManagers::CAbstractNodeEvaluator *node_evaluator);
//	template <u64 flags>
//			void	select_location			(PathManagers::CNodeEvaluator<flags> *node_evaluator);
			void	select_level_vertex		();
			bool	level_vertex_selection_actual();
};
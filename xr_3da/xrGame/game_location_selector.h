////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_selector.h"

class CGameLocationSelector : virtual public CGameObject {
protected:
	u32						m_game_locate_query_time;
	u32						m_game_location_query_interval;
	bool					m_game_selector_failed;
	CAbstractNodeEvaluator	*m_game_node_evaluator;

public:
					CGameLocationSelector	();
	virtual			~CGameLocationSelector	();
	virtual void	init_evaluator			(PathManagers::CAbstractNodeEvaluator *node_evaluator);
	template <u64 flags>
			void	select_location			(PathManagers::CNodeEvaluator<flags> *node_evaluator);
};
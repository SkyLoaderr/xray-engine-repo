////////////////////////////////////////////////////////////////////////////
//	Module 		: game_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_selector.h"

class CGamePathManager {
protected:
	bool									m_game_path_actual;
			bool	valid					() const;
			bool	valid					(u32 node_id) const;
public:
	enum EGamePathType {
		eGamePathTypeMinTime = u32(0),
	};

	xr_vector<u32>							m_game_path;
	u32										m_game_start_node;
	u32										m_game_dest_node;
	u32										m_game_cur_node_index;
	EGamePathType							m_game_path_type;
	PathManagers::CAbstractNodeEvaluator	*m_game_path_evaluator;


					CGamePathManager			();
	virtual			~CGamePathManager			();
	virtual void	init						();
			void	build_game_path				();
};


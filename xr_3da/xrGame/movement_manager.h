////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "location_selector.h"

class CPHMovementControl;

class CMovementManager : 
	public CGamePathManager,
	public CLevelPathManager,
	public CDetailPathManager,
	public CLocationSelector
{
private:
	enum EPathState {
		ePathStateBuildGamePath = u32(0),
		ePathStateBuildLevelPath,
		ePathStateBuildDetailPath,
		ePathStateSearchGameNode,
		ePathStateSearchLevelNode,
		ePathStateDummy = u32(-1),
	};
	
	float									m_cumulative_time_delta;
	EPathState								m_path_state;
	EPathState								m_path_state_previous;

	void			init_selector			(PathManagers::CAbstractNodeEvaluator &node_evaluator, CSquad &squad) const;
	void			find_position			(PathManagers::CAbstractNodeEvaluator &node_evaluator, CSquad &squad);

protected:
	float									m_speed;

public:
					CMovementManager		();
	virtual			~CMovementManager		();
	virtual void	init					();
			void	build_path				(PathManagers::CAbstractNodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode);
			void	move_along_path			(CPHMovementControl *movement_control, float time_delta);
};
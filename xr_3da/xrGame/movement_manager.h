////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_location_selector.h"
#include "game_path_manager.h"
#include "level_location_selector.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "enemy_predictor.h"

class CPHMovementControl;

class CMovementManager : 
	public CGameLocationSelector,
	public CGamePathManager,
	public CLevelLocationSelector,
	public CLevelPathManager,
	public CDetailPathManager,
	public CEnemyPredictor
{
private:
	enum EPathState {
		ePathStateSelectGameVertex = u32(0),
		ePathStateBuildGamePath,
		ePathStateContinueGamePath,
		
		ePathStatePredictEnemyVertices,
		ePathStateSelectEnemyVertex,
		ePathStateContinueEnemySearch,

		ePathStateSelectLevelVertex,
		ePathStateBuildLevelPath,
		ePathStateContinueLevelPath,
		
		ePathStateBuildDetailPath,
		
		ePathStatePathVerification,
		
		ePathStatePathCompleted,
		
		ePathStateDummy = u32(-1),
	};

public:
	enum EPathType {
		ePathTypeGamePath = u32(0),
		ePathTypeLevelPath,
		ePathTypeEnemySearch,
		ePathTypeDummy = u32(-1),
	};

private:
	EPathState								m_path_state;
	EPathType								m_path_type;
	EPathType								m_path_type_previous;

//	template <u64 flags>
//	void			find_location			(PathManagers::CNodeEvaluator<flags> *node_evaluator);
	void			process_game_path		();
	void			process_level_path		();
	void			process_enemy_search	();

protected:
	float									m_speed;

public:
					CMovementManager		();
	virtual			~CMovementManager		();
	virtual void	init					();
			void	build_path				();
			void	move_along_path			(CPHMovementControl *movement_control, float time_delta);
};
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
#include "enemy_predictor.h"

class CPHMovementControl;

class CMovementManager : 
	public CGamePathManager,
	public CLevelPathManager,
	public CDetailPathManager,
	public CLocationSelector,
	public CEnemyPredictor
{
private:
	enum EPathState {
		ePathStateSelectGameVertex = u32(0),
		ePathStateBuildGamePath,
		ePathStateSelectLevelVertex,
		ePathStateBuildLevelPath,
		ePathStateBuildDetailPath,
		ePathStatePathVerification,
		ePathStatePredictEnemyVertices,
		ePathStateSelectEnemyVertex,
		ePathStateDummy = u32(-1),
	};
	
	enum EPathType {
		ePathTypeGamePath = u32(0),
		ePathTypeLevelPath,
		ePathTypeEnemySearch,
		ePathStateDummy = u32(-1),
	};
	
	EPathState								m_path_state;
	EPathType								m_path_type;
	EPathType								m_path_type_previous;

	template <u64 flags>
	void			find_location			(PathManagers::CNodeEvaluator<flags> *node_evaluator);

protected:
	float									m_speed;

public:
					CMovementManager		();
	virtual			~CMovementManager		();
	virtual void	init					();
			void	build_path				(PathManagers::CAbstractNodeEvaluator *node_evaluator, const Fvector *target_position);
			void	move_along_path			(CPHMovementControl *movement_control, float time_delta);
};
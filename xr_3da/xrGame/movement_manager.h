////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_game_selector.h"
#include "path_manager_level_selector.h"
#include "abstract_location_selector.h"
#include "abstract_path_manager.h"
#include "game_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "enemy_location_predictor.h"
#include "gameobject.h"

class CPHMovementControl;

class CMovementManager : 
	public CBaseLocationSelector<CGameGraph,PathManagers::SVertexType<float,u32,u32>,u32>,
	public CBasePathManager		<CGameGraph,PathManagers::SBaseParameters<float,u32,u32>,u32,u32>,
	public CBaseLocationSelector<CLevelGraph,PathManagers::CAbstractVertexEvaluator,u32>,
	public CBasePathManager		<CLevelGraph,PathManagers::SBaseParameters<float,u32,u32>,u32,u32>,
	public CDetailPathManager,
	public CEnemyLocationPredictor,
	virtual public CGameObject
{
protected:
	typedef CBaseLocationSelector	<CGameGraph,PathManagers::SVertexType<float,u32,u32>,u32>			CGameLocationSelector;
	typedef CBasePathManager		<CGameGraph,PathManagers::SBaseParameters<float,u32,u32>,u32,u32>	CGamePathManager;
	typedef CBaseLocationSelector	<CLevelGraph,PathManagers::CAbstractVertexEvaluator,u32>			CLevelLocationSelector;
	typedef CBasePathManager		<CLevelGraph,PathManagers::SBaseParameters<float,u32,u32>,u32,u32>	CLevelPathManager;

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
	bool									m_path_actuality;
	bool									m_enabled;
	u64										m_start_time;
	u64										m_time_work;
	CGraphEngine::CBaseParameters			*m_base_game_selector;
	CGraphEngine::CBaseParameters			*m_base_level_selector;
	float									m_speed;

	IC		void	time_start				();
	IC		bool	time_over				() const;
	IC		bool	actual					() const;
			void	process_game_path		();
			void	process_level_path		();
			void	process_enemy_search	();

	friend class CScriptMonster;
	friend class CGroup;

public:
					CMovementManager		();
	virtual			~CMovementManager		();
	virtual void	Init					();
	IC		void	set_path_type			(EPathType path_type);
	IC		void	set_game_dest_vertex	(const ALife::_GRAPH_ID game_vertex_id);
	IC		void	set_level_dest_vertex	(const u32 level_vertex_id);
	IC		void	enable_movement			(bool enabled);
	IC		EPathType path_type				() const;
	IC		ALife::_GRAPH_ID game_dest_vertex_id	() const;
	IC		u32		level_dest_vertex_id	() const;
	IC		bool	enabled					() const;

			void	build_path				();
			void	move_along_path			(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta);
			float	speed					() const;
	IC		bool	path_completed			() const;
};

#include "movement_manager_inline.h"
